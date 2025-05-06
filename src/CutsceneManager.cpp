#include "CutsceneManager.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
}

struct CutsceneManager::Impl
{
    AVFormatContext *fmtCtx;
    AVCodecContext *codecCtx;
    const AVCodec *codec;
    AVFrame *frame;
    AVFrame *frameRGBA;
    struct SwsContext *swsCtx;
    int videoStreamIndex;
    bool videoLoaded;
    bool playing;
    bool finished;
    SDL_Texture *texture;
    int videoWidth;
    int videoHeight;
    int frameDelay;
};

CutsceneManager::CutsceneManager()
{
    pImpl = new Impl();
    pImpl->fmtCtx = nullptr;
    pImpl->codecCtx = nullptr;
    pImpl->codec = nullptr;
    pImpl->frame = nullptr;
    pImpl->frameRGBA = nullptr;
    pImpl->swsCtx = nullptr;
    pImpl->videoStreamIndex = -1;
    pImpl->videoLoaded = false;
    pImpl->playing = false;
    pImpl->finished = false;
    pImpl->texture = nullptr;
    pImpl->videoWidth = 0;
    pImpl->videoHeight = 0;
    pImpl->frameDelay = 0;
}

CutsceneManager::~CutsceneManager()
{
    if (pImpl->texture)
        SDL_DestroyTexture(pImpl->texture);
    if (pImpl->frameRGBA)
    {
        if (pImpl->frameRGBA->data[0])
            av_freep(&pImpl->frameRGBA->data[0]);
        av_frame_free(&pImpl->frameRGBA);
    }
    if (pImpl->frame)
        av_frame_free(&pImpl->frame);
    if (pImpl->codecCtx)
        avcodec_free_context(&pImpl->codecCtx);
    if (pImpl->fmtCtx)
        avformat_close_input(&pImpl->fmtCtx);
    if (pImpl->swsCtx)
        sws_freeContext(pImpl->swsCtx);
    delete pImpl;
}

bool CutsceneManager::loadVideo(const std::string &filePath, SDL_Renderer *renderer)
{
    // tell FFmpeg to probe more of the file so it actually learns the video size & pix_fmt
    AVDictionary *opts = nullptr;
    // increase probe size to ~5MB
    av_dict_set(&opts, "probesize", "5242880", 0);
    // increase max analyze duration to 1 second
    av_dict_set(&opts, "max_analyze_duration", "1000000", 0);

    if (avformat_open_input(&pImpl->fmtCtx, filePath.c_str(), nullptr, &opts) != 0)
    {
        std::cerr << "Could not open video file: " << filePath << "\n";
        av_dict_free(&opts);
        return false;
    }
    av_dict_free(&opts);

    if (avformat_find_stream_info(pImpl->fmtCtx, nullptr) < 0)
    {
        std::cerr << "Could not find stream info: " << filePath << "\n";
        return false;
    }

    // find the video stream index
    for (unsigned i = 0; i < pImpl->fmtCtx->nb_streams; ++i)
    {
        if (pImpl->fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            pImpl->videoStreamIndex = i;
            break;
        }
    }
    if (pImpl->videoStreamIndex < 0)
    {
        std::cerr << "No video stream in: " << filePath << "\n";
        return false;
    }

    // get codec parameters
    AVCodecParameters *params = pImpl->fmtCtx->streams[pImpl->videoStreamIndex]->codecpar;
    if (params->width <= 0 || params->height <= 0)
    {
        std::cerr << "Invalid video dimensions: "
                  << params->width << "x" << params->height
                  << " in " << filePath << "\n";
        return false;
    }

    pImpl->videoWidth = params->width;
    pImpl->videoHeight = params->height;

    // find and open decoder
    pImpl->codec = avcodec_find_decoder(params->codec_id);
    if (!pImpl->codec)
    {
        std::cerr << "Unsupported codec in: " << filePath << "\n";
        return false;
    }
    pImpl->codecCtx = avcodec_alloc_context3(pImpl->codec);
    avcodec_parameters_to_context(pImpl->codecCtx, params);
    if (avcodec_open2(pImpl->codecCtx, pImpl->codec, nullptr) < 0)
    {
        std::cerr << "Could not open codec for: " << filePath << "\n";
        return false;
    }

    // allocate frames
    pImpl->frame = av_frame_alloc();
    pImpl->frameRGBA = av_frame_alloc();

    // allocate buffer for RGBA
    int bufSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA,
                                           pImpl->videoWidth,
                                           pImpl->videoHeight, 1);
    uint8_t *buffer = (uint8_t *)av_malloc(bufSize);
    av_image_fill_arrays(pImpl->frameRGBA->data,
                         pImpl->frameRGBA->linesize,
                         buffer,
                         AV_PIX_FMT_RGBA,
                         pImpl->videoWidth,
                         pImpl->videoHeight,
                         1);

    // make SDL texture
    pImpl->texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        pImpl->videoWidth,
        pImpl->videoHeight);
    if (!pImpl->texture)
    {
        std::cerr << "SDL_CreateTexture: " << SDL_GetError() << "\n";
        return false;
    }

    // compute frame delay
    AVRational fr = av_guess_frame_rate(pImpl->fmtCtx,
                                        pImpl->fmtCtx->streams[pImpl->videoStreamIndex],
                                        nullptr);
    pImpl->frameDelay = (fr.num && fr.den)
                            ? static_cast<int>(1000 * av_q2d(av_inv_q(fr)))
                            : 33;

    pImpl->videoLoaded = true;
    return true;
}

void CutsceneManager::play()
{
    if (!pImpl->videoLoaded)
        return;
    av_seek_frame(pImpl->fmtCtx, pImpl->videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(pImpl->codecCtx);
    pImpl->playing = true;
    pImpl->finished = false;
}

bool CutsceneManager::update()
{
    if (!pImpl->playing)
        return pImpl->finished;

    AVPacket *pkt = av_packet_alloc();
    bool gotFrame = false;

    while (av_read_frame(pImpl->fmtCtx, pkt) >= 0)
    {
        if (pkt->stream_index == pImpl->videoStreamIndex)
        {
            if (avcodec_send_packet(pImpl->codecCtx, pkt) == 0 &&
                avcodec_receive_frame(pImpl->codecCtx, pImpl->frame) == 0)
            {
                // **lazy‐init** scaler the first time we know the real pix_fmt
                if (!pImpl->swsCtx)
                {
                    pImpl->swsCtx = sws_getContext(
                        pImpl->frame->width,
                        pImpl->frame->height,
                        (AVPixelFormat)pImpl->frame->format,
                        pImpl->videoWidth,
                        pImpl->videoHeight,
                        AV_PIX_FMT_RGBA,
                        SWS_BILINEAR,
                        nullptr, nullptr, nullptr);
                    if (!pImpl->swsCtx)
                    {
                        std::cerr << "Failed to create sws context on first frame\n";
                        av_packet_unref(pkt);
                        break;
                    }
                }
                sws_scale(
                    pImpl->swsCtx,
                    (uint8_t const *const *)pImpl->frame->data,
                    pImpl->frame->linesize,
                    0,
                    pImpl->frame->height,
                    pImpl->frameRGBA->data,
                    pImpl->frameRGBA->linesize);

                SDL_UpdateTexture(pImpl->texture,
                                  nullptr,
                                  pImpl->frameRGBA->data[0],
                                  pImpl->frameRGBA->linesize[0]);
                gotFrame = true;
                av_packet_unref(pkt);
                break;
            }
        }
        av_packet_unref(pkt);
    }
    av_packet_free(&pkt);

    if (!gotFrame)
    {
        pImpl->playing = false;
        pImpl->finished = true;
    }
    else
    {
        SDL_Delay(pImpl->frameDelay);
    }
    return pImpl->finished;
}

void CutsceneManager::render(SDL_Renderer *renderer)
{
    if (!pImpl->videoLoaded)
        return;
    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    SDL_Rect dst{0, 0, w, h};
    SDL_RenderCopy(renderer, pImpl->texture, nullptr, &dst);
}

bool CutsceneManager::isFinished() const
{
    return pImpl->finished;
}

void CutsceneManager::skip()
{
    pImpl->playing = false;
    pImpl->finished = true;
}