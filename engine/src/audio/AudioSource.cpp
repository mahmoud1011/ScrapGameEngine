#include "audio/AudioSource.h"
#include "audio/AudioDevice.h"

#include <miniaudio.h>

#include <algorithm>
#include <iostream>

using namespace ScrapGameEngine;

namespace
{
    inline ma_sound* asSound(void* p) { return static_cast<ma_sound*>(p); }
}

AudioSource::AudioSource(GameObject* owner)
    : BaseComponent(owner), sound(nullptr), looping(false), volume(1.0f)
{
}

AudioSource::~AudioSource()
{
    unload();
}

bool AudioSource::load(const std::string& path, bool loop)
{
    unload();

    if (path.empty())
    {
        std::cerr << "[AUDIO] load() called with an empty path." << std::endl;
        return false;
    }

    if (!AudioDevice::ensureStarted())
    {
        return false;
    }

    auto* engine = static_cast<ma_engine*>(AudioDevice::nativeHandle());
    auto* s = new ma_sound{};

    if (ma_sound_init_from_file(engine, path.c_str(), 0, nullptr, nullptr, s) != MA_SUCCESS)
    {
        std::cerr << "[AUDIO] Could not load '" << path << "'." << std::endl;
        delete s;
        return false;
    }

    sound = s;
    filePath = path;
    looping = loop;

    ma_sound_set_looping(s, loop ? MA_TRUE : MA_FALSE);
    ma_sound_set_volume(s, volume);
    return true;
}

void AudioSource::play()
{
    if (!sound)
    {
        std::cerr << "[AUDIO] play() called before a file was loaded." << std::endl;
        return;
    }

    ma_sound_seek_to_pcm_frame(asSound(sound), 0);
    ma_sound_start(asSound(sound));
}

void AudioSource::pause()
{
    if (sound) ma_sound_stop(asSound(sound));
}

void AudioSource::stop()
{
    if (!sound) return;

    ma_sound_stop(asSound(sound));
    ma_sound_seek_to_pcm_frame(asSound(sound), 0);
}

void AudioSource::setVolume(float value)
{
    volume = std::clamp(value, 0.0f, 1.0f);
    if (sound) ma_sound_set_volume(asSound(sound), volume);
}

float AudioSource::getVolume() const
{
    return volume;
}

void AudioSource::setLooping(bool loop)
{
    looping = loop;
    if (sound) ma_sound_set_looping(asSound(sound), loop ? MA_TRUE : MA_FALSE);
}

bool AudioSource::isLooping() const
{
    return looping;
}

bool AudioSource::isPlaying() const
{
    return sound && ma_sound_is_playing(asSound(sound)) == MA_TRUE;
}

void AudioSource::unload()
{
    if (!sound) return;

    ma_sound_uninit(asSound(sound));
    delete asSound(sound);
    sound = nullptr;
}
