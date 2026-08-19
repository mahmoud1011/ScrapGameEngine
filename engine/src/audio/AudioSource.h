#pragma once

#include "scene/BaseComponent.h"
#include <string>

namespace ScrapGameEngine
{
    /**
     * @class AudioSource
     * @brief Plays a sound file, attached to a GameObject.
     *
     * Every AudioSource mixes into the process-wide AudioDevice rather than owning an
     * engine of its own. The sound is decoded when load() is called, so play() is cheap
     * and can be called repeatedly.
     */
    class AudioSource : public BaseComponent
    {
    public:
        explicit AudioSource(GameObject* owner);
        ~AudioSource() override;

        AudioSource(const AudioSource&) = delete;
        AudioSource& operator=(const AudioSource&) = delete;

        /**
         * @brief Loads an audio file, replacing anything previously loaded.
         * @param filePath Path to the audio file, relative to the working directory.
         * @param loop Whether playback should repeat.
         * @return True if the file was decoded successfully.
         */
        bool load(const std::string& filePath, bool loop = false);

        /** @brief Starts playback from the beginning. */
        void play();

        /** @brief Halts playback, keeping the current position. */
        void pause();

        /** @brief Halts playback and rewinds to the start. */
        void stop();

        /** @brief Sets the volume, clamped to [0, 1]. */
        void setVolume(float volume);
        float getVolume() const;

        void setLooping(bool loop);
        bool isLooping() const;

        bool isPlaying() const;

        /** @brief Releases the decoded sound. Safe to call more than once. */
        void unload();

    private:
        void* sound;          ///< Opaque ma_sound; heap-allocated so miniaudio stays out of this header.
        std::string filePath;
        bool looping;
        float volume;
    };
}
