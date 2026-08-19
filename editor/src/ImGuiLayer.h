#pragma once

struct GLFWwindow;

namespace Scrap::Editor
{
    /**
     * @class ImGuiLayer
     * @brief Owns the ImGui context and its GLFW/OpenGL3 backends.
     *
     * Wraps setup so the editor's main loop reads as begin/end rather than backend
     * calls, and so docking and viewport flags live in one place.
     */
    class ImGuiLayer
    {
    public:
        ImGuiLayer() = delete;

        /**
         * @brief Creates the ImGui context and attaches it to the window.
         * @return False if either backend refuses to initialise.
         */
        static bool init(GLFWwindow* window);
        static void shutdown();

        /** @brief Starts an ImGui frame. */
        static void begin();

        /**
         * @brief Renders the frame's draw data.
         *
         * With multi-viewport enabled this also updates the platform windows, which
         * rebinds the GL context - anything drawing afterwards must rebind its own.
         */
        static void end(int displayWidth, int displayHeight);

        /** @brief Submits the full-window dockspace the panels dock into. */
        static void beginDockspace();
        static void endDockspace();

        /** @brief True while ImGui wants the keyboard, so the editor should not act on it. */
        static bool wantsKeyboard();

        /** @brief True while ImGui wants the mouse. */
        static bool wantsMouse();
    };
}
