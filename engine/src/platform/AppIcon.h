#pragma once

#include <string>

struct GLFWwindow;

namespace ScrapGameEngine
{
    /**
     * @class AppIcon
     * @brief Sets the window icon shown in the title bar, taskbar and alt-tab.
     *
     * Separate from the executable icon, which is a Windows resource compiled into the
     * binary and is what Explorer shows. Both are needed: the resource icon identifies
     * the file, this one identifies the running window.
     */
    class AppIcon
    {
    public:
        AppIcon() = delete;

        /**
         * @brief Loads a PNG and applies it as the window icon.
         * @return False if the file is missing or cannot be decoded; the window simply
         *         keeps the default icon rather than failing to open.
         *
         * The image is downscaled to the sizes desktops actually ask for, so the
         * taskbar is not left resampling a large source every time it draws. A
         * non-square source is letterboxed rather than stretched - squashing a logo
         * to fit looks worse than padding it.
         *
         * No-op on macOS, where the icon comes from the application bundle and GLFW
         * ignores this call by design.
         */
        static bool setFromPng(GLFWwindow* window, const std::string& pngPath);

        /**
         * @brief Finds the branding icon relative to the executable.
         *
         * Resources are staged beside the binary at build time, but running from a
         * source tree is common enough to be worth handling, so a couple of parent
         * directories are checked too.
         */
        static std::string locateBrandingIcon();
    };
}
