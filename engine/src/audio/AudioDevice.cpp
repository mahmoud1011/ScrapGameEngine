#include "audio/AudioDevice.h"

// This is the one translation unit that compiles miniaudio itself.
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <iostream>

using namespace ScrapGameEngine;

namespace
{
    ma_engine engine;
    bool started = false;
    bool failed = false;
}

bool AudioDevice::ensureStarted()
{
    if (started) return true;
    if (failed) return false;

    if (ma_engine_init(nullptr, &engine) != MA_SUCCESS)
    {
        std::cerr << "[AUDIO] Could not initialise the audio device. "
                     "Sound is disabled for this session." << std::endl;
        failed = true;
        return false;
    }

    started = true;
    return true;
}

void AudioDevice::shutdown()
{
    if (!started) return;

    ma_engine_uninit(&engine);
    started = false;
}

void* AudioDevice::nativeHandle()
{
    return started ? &engine : nullptr;
}
