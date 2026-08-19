#pragma once

namespace ScrapGameEngine
{
    /**
     * @class AudioDevice
     * @brief Owns the single process-wide audio engine that every AudioSource mixes into.
     *
     * The 0.7 AudioSource spun up a complete sound engine per component, which meant
     * one output device, mixing graph and worker thread for every sound in the scene.
     * The device is now shared: it starts lazily on first use and is torn down once at
     * shutdown.
     *
     * The backing implementation is deliberately not exposed here. Audio pulls in a
     * very large single-header library, and keeping it out of engine headers keeps it
     * out of every translation unit that merely wants to play a sound.
     */
    class AudioDevice
    {
    public:
        AudioDevice() = delete;

        /**
         * @brief Starts the audio engine if it is not already running.
         * @return True if the engine is usable, false if initialisation failed.
         *
         * Safe to call repeatedly. A failure is reported once and then latched, so a
         * machine with no audio output does not spam the log every frame.
         */
        static bool ensureStarted();

        /**
         * @brief Shuts the audio engine down and releases the output device.
         *
         * Called by Application during cleanup. Any sound still playing is stopped.
         */
        static void shutdown();

        /**
         * @brief Returns the native engine handle, or nullptr if it is not running.
         *
         * For use by AudioSource only; the type is opaque outside the audio module.
         */
        static void* nativeHandle();
    };
}
