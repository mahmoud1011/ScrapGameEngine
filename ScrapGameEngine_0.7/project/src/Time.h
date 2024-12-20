#pragma once
namespace ScrapGameEngine
{
    namespace Time
    {
        /**
         * @brief Get the current time in seconds.
         *
         * This function retrieves the current time in seconds since the application started.
         *
         * @return The current time as a float.
         */
        float getTime();

        /**
         * @brief Get the time difference between frames.
         *
         * This function calculates the time difference between the current frame and the previous frame, commonly known as the delta time.
         *
         * @return The delta time in seconds as a float.
         */
        float getDeltaTime();

        /**
         * @brief Process the time for the current frame.
         *
         * This function updates the time state with the given frame time. It should be called each frame during the application loop to manage time-based behaviors.
         *
         * @param frameTime The time taken to process the current frame in seconds.
         */
        void processTime(float frameTime);
    };
}
