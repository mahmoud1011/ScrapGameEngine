#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "BaseComponent.h"
#include <string>
#include <irrKlang.h>

using namespace irrklang;

namespace ScrapGameEngine
{
    /**
     * @class AudioSource
     * @brief Represents an audio source component for playing and managing audio files.
     *
     * This class provides functionalities to load audio files, play, pause, stop audio playback,
     * control volume, and looping behavior. It uses the irrKlang sound engine to handle audio.
     */
    class AudioSource : public BaseComponent
    {
    public:
        /**
         * @brief Constructs an AudioSource instance attached to the given GameObject.
         * @param owner The GameObject to which this AudioSource component is attached.
         */
        explicit AudioSource(GameObject* owner);

        /**
         * @brief Loads an audio file from the specified file path.
         * @param filePath The path to the audio file.
         * @param loop Specifies whether the audio should loop.
         */
        void load(const std::string& filePath, bool loop = false);

        /**
         * @brief Starts or resumes audio playback.
         */
        void play();

        /**
         * @brief Pauses audio playback.
         */
        void pause();

        /**
         * @brief Stops audio playback.
         */
        void stop();

        /**
         * @brief Sets the volume of the audio.
         * @param volume The volume level (0.0 to 1.0).
         */
        void setVolume(float volume);

        /**
         * @brief Gets the current volume of the audio.
         * @return The current volume level.
         */
        float getVolume() const;

        /**
         * @brief Sets whether the audio should loop when finished.
         * @param loop Specifies if the audio should loop.
         */
        void setLooping(bool loop);

        /**
         * @brief Checks if the audio is currently looping.
         * @return True if the audio is looping, false otherwise.
         */
        bool isLooping() const;

        /**
         * @brief Checks if the audio is currently playing.
         * @return True if the audio is playing, false otherwise.
         */
        bool isPlaying() const;

        /**
         * @brief Updates the AudioSource, typically called once per frame.
         * @param deltaTime The time elapsed since the last frame.
         */
        void update(float deltaTime) override;

        /**
         * @brief Cleans up resources used by the AudioSource.
         */
        void cleanup();

    private:
        /**
         * @brief Destructor for AudioSource.
         * Cleans up the sound engine and current sound.
         */
        ~AudioSource();

        irrklang::ISoundEngine* _soundEngine; ///< The sound engine instance.
        irrklang::ISound* _currentSound; ///< The currently loaded sound.
        std::string _filePath; ///< The path to the audio file.
        bool _looping; ///< Indicates whether the audio should loop.
        float _volume; ///< The current volume level.
    };
}

#endif // AUDIO_SOURCE_H
