// SoundManager.h
#pragma once

#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

class SoundManager
{
public:
    // Initialize the audio subsystem. Returns false on failure.
    static bool init(int frequency = 44100, Uint16 format = MIX_DEFAULT_FORMAT, int channels = 2, int chunksize = 2048);

    // Load a sound effect (short) from file. Returns true on success.
    bool loadEffect(const std::string &id, const std::string &filepath);

    // Load music (longer, e.g. mp3) from file.
    bool loadMusic(const std::string &id, const std::string &filepath);

    // Play a sound effect once (channel -1 picks first free channel).
    void playEffect(const std::string &id, int loops = 0);

    // Play background music. loops=-1 for infinite loop.
    void playMusic(const std::string &id, int loops = -1);

    // Stop music playback immediately.
    void stopMusic();

    // Set music volume [0..MIX_MAX_VOLUME]
    void setMusicVolume(int volume);

    // Clean up all loaded resources and close audio.
    void cleanup();

    void setEffectVolume(const std::string &id, int volume);

private:
    std::unordered_map<std::string, Mix_Chunk *> effects_;
    std::unordered_map<std::string, Mix_Music *> music_;
};