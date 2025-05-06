#ifndef CUTSCENE_MANAGER_H
#define CUTSCENE_MANAGER_H

#include <string>
#include <SDL2/SDL.h>

// The CutsceneManager uses FFmpeg to decode a video and display its frames as SDL_Textures.
class CutsceneManager
{
public:
    CutsceneManager();
    ~CutsceneManager();

    // Load the video file into memory and prepare for playback.
    bool loadVideo(const std::string &filePath, SDL_Renderer *renderer);

    // Starts (or resumes) the cutscene playback.
    void play();

    // Decode the next frame(s) of the video.
    bool update();

    // Render the current video frame.
    void render(SDL_Renderer *renderer);

    // Returns true if the video has finished playing.
    bool isFinished() const;

    // Force skip (end) the cutscene.
    void skip();

private:
    // Private implementation structure (PIMPL) to hide FFmpeg details.
    struct Impl;
    Impl *pImpl;
};

#endif // CUTSCENE_MANAGER_H
