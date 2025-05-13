// SoundManager.cpp
#include "SoundManager.h"
#include <iostream>

bool SoundManager::init(int frequency, Uint16 format, int channels, int chunksize)
{
    if (Mix_OpenAudio(frequency, format, channels, chunksize) < 0)
    {
        std::cerr << "SDL_mixer Init Error: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_AllocateChannels(32);
    return true;
}

bool SoundManager::loadEffect(const std::string &id, const std::string &filepath)
{
    Mix_Chunk *chunk = Mix_LoadWAV(filepath.c_str());
    if (!chunk)
    {
        std::cerr << "Failed to load SFX '" << id << "': " << Mix_GetError() << std::endl;
        return false;
    }
    effects_[id] = chunk;
    return true;
}

bool SoundManager::loadMusic(const std::string &id, const std::string &filepath)
{
    Mix_Music *mus = Mix_LoadMUS(filepath.c_str());
    if (!mus)
    {
        std::cerr << "Failed to load music '" << id << "': " << Mix_GetError() << std::endl;
        return false;
    }
    music_[id] = mus;
    return true;
}

void SoundManager::playEffect(const std::string &id, int loops)
{
    auto it = effects_.find(id);
    if (it != effects_.end())
    {
        Mix_PlayChannel(-1, it->second, loops);
    }
}

void SoundManager::playMusic(const std::string &id, int loops)
{
    auto it = music_.find(id);
    if (it != music_.end())
    {
        Mix_PlayMusic(it->second, loops);
    }
}

void SoundManager::stopMusic()
{
    Mix_HaltMusic();
}

void SoundManager::setMusicVolume(int volume)
{
    Mix_VolumeMusic(volume);
}

void SoundManager::cleanup()
{
    for (auto &p : effects_)
    {
        Mix_FreeChunk(p.second);
    }
    effects_.clear();
    for (auto &p : music_)
    {
        Mix_FreeMusic(p.second);
    }
    music_.clear();
    Mix_CloseAudio();
}

void SoundManager::setEffectVolume(const std::string &id, int volume)
{
    auto it = effects_.find(id);
    if (it != effects_.end())
        Mix_VolumeChunk(it->second, volume);
}
