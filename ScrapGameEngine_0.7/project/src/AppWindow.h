#pragma once
#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace ScrapGameEngine
{
    /**
     * @enum AppWindowEventType
     * @brief Enum representing different types of window events.
     */
    enum class AppWindowEventType
    {
        CLOSE,               /**< The window close event. */
        FRAMEBUFFER_RESIZE   /**< The window framebuffer resize event. */
    };

    /**
     * @typedef AppWindowEventCallbackFn
     * @brief A callback function type for handling window events.
     * @param type The type of the event.
     * @param data Pointer to additional data associated with the event.
     */
    using AppWindowEventCallbackFn = std::function<void(AppWindowEventType, void*)>;

    /**
     * @struct AppWindowData
     * @brief Data structure for storing window properties.
     */
    struct AppWindowData
    {
        int width;              /**< The width of the window. */
        int height;             /**< The height of the window. */
        std::string title;      /**< The title of the window. */

        /**
         * @brief Constructor to initialize the AppWindowData structure.
         * @param width The width of the window.
         * @param height The height of the window.
         * @param title The title of the window.
         */
        AppWindowData(int width, int height, std::string title)
            : width(width), height(height), title(std::move(title)) {}

        AppWindowEventCallbackFn func_cb; /**< The callback function for window events. */
    };

    /**
     * @class AppWindow
     * @brief Represents a graphical application window.
     */
    class AppWindow
    {
    private:
        AppWindowData windowData; /**< The data associated with the window. */
        void* nativeWindow;      /**< Pointer to the native window handle. */

    public:
        /**
         * @brief Constructor that accepts an AppWindowData structure.
         * @param data The window data to initialize with.
         */
        AppWindow(const AppWindowData& data);

        /**
         * @brief Destructor for cleaning up resources.
         */
        ~AppWindow();

        /**
         * @brief Retrieves the native window handle.
         * @return The native window handle.
         */
        void* getNativeWindow();

        /**
         * @brief Initializes the window with the given data.
         * @param data The data to initialize the window with.
         * @return Status code (0 for success, non-zero for failure).
         */
        int init(AppWindowData data);

        /**
         * @brief Updates the window state.
         */
        void update();

        /**
         * @brief Sets the callback function for window events.
         * @param fn The callback function to set.
         */
        void setWindowEventCallback(AppWindowEventCallbackFn fn);

        /**
         * @brief Sets the size of the window.
         * @param width The new width of the window.
         * @param height The new height of the window.
         */
        void setSize(int width, int height);

        /**
         * @brief Retrieves the screen size as a 2D vector.
         * @return The screen size.
         */
        glm::vec2 getScreenSize() const;
    };
}
