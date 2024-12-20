#pragma once
#include "AppWindow.h"
#include <string>

namespace ScrapGameEngine
{
    /**
     * @class Application
     * @brief Manages the application lifecycle, including window creation, event processing, and main game loop.
     *
     * The `Application` class handles the initialization, running, and cleanup of the game application.
     * It is responsible for managing the main application window, setting the target frame rate, and controlling
     * the main game loop. It also handles window events such as resizing or closing.
     */
    class Application
    {
    public:
        /**
         * @brief Constructs an `Application` instance.
         * @param width The width of the application window.
         * @param height The height of the application window.
         * @param title The title of the application window.
         */
        Application(int width, int height, const std::string& title)
            : windowData(width, height, title), window(windowData), targetFrameRate(0), frameTime(0.0f), isRunning(false) {}

        /**
         * @brief Retrieves the singleton instance of the `Application`.
         * @return Pointer to the `Application` instance.
         */
        static Application* getInstance() { return instance; }

        /**
         * @brief Initializes the application.
         * @return 0 on success, non-zero error code on failure.
         */
        int init();

        /**
         * @brief Runs the main application loop.
         */
        void run();

        /**
         * @brief Cleans up resources before exiting the application.
         */
        void cleanup();

        /**
         * @brief Sets the target frame rate for the application.
         * @param frameRate The desired frame rate in frames per second.
         */
        static void setTargetFrameRate(unsigned int frameRate);

        /**
         * @brief Quits the application, triggering cleanup and exit processes.
         */
        static void quit();

        /**
         * @brief Processes a window event.
         * @param eventType The type of the window event.
         * @param payload Additional information associated with the event.
         */
        void processWindowEvent(AppWindowEventType eventType, void* payload);

        /**
         * @brief Provides access to the application window.
         * @return A reference to the `AppWindow`.
         */
        AppWindow& getWindow() { return window; }

    private:
        static Application* instance; /**< Singleton instance of the `Application`. */
        AppWindow window; /**< The main application window. */
        AppWindowData windowData; /**< Data describing the main application window. */

        unsigned int targetFrameRate; /**< Desired frame rate in frames per second. */
        float frameTime; /**< Time per frame in seconds. */
        bool isRunning; /**< Flag indicating if the application is currently running. */
    };
}
