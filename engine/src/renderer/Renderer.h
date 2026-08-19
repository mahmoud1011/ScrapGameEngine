#pragma once

#include <glm/glm.hpp>

namespace ScrapGameEngine
{
    class Texture2D;
    class Framebuffer;

    /**
     * @struct DrawCommand
     * @brief A single quad submission.
     *
     * Kept as the submission shape Graphics::drawMesh produces, but it no longer
     * describes a mesh binding - the batcher generates the geometry, so only the
     * transform, tint and texture matter.
     */
    struct DrawCommand
    {
        glm::vec4 tint{1.0f};
        glm::vec3 translation{0.0f};
        float rotationZ = 0.0f;
        glm::vec3 scale{1.0f};
        Texture2D* texture = nullptr;
    };

    /**
     * @class Renderer
     * @brief Frame lifecycle and render state.
     *
     * Owns the offscreen target every frame is drawn into, then blits it to the
     * window. The runtime pays a blit for that; in exchange the editor can hand the
     * same colour attachment to a viewport panel without the renderer knowing.
     */
    class Renderer
    {
    public:
        Renderer() = delete;

        /** @brief Brings up render state, the batcher and the scene target. */
        static bool init(unsigned int width, unsigned int height);
        static void shutdown();

        /** @brief Binds the scene target and starts a batch, using the engine Camera. */
        static void beginFrame();

        /** @brief Flushes the batch and blits the scene target to the window. */
        static void endFrame();

        /**
         * @brief Starts a frame against a caller-supplied view-projection.
         *
         * The editor needs this: its camera must drive the scene pass while the game's
         * Camera stays untouched. Once Camera becomes a component this stops being a
         * separate entry point and is simply how every frame begins.
         */
        static void beginFrameWith(const glm::mat4& viewProjection);

        /**
         * @brief Flushes the batch and leaves the result in the scene target.
         *
         * No blit to the window, because the caller is going to sample the colour
         * attachment itself - an editor viewport panel rather than a fullscreen game.
         */
        static void endFrameOffscreen();

        /** @brief Queues a quad. Only valid between beginFrame and endFrame. */
        static void submitCommand(const DrawCommand& dc);

        static void setViewport(int x, int y, int width, int height);
        static void setClearColor(float r, float g, float b, float a);
        static void clear();

        /** @brief The offscreen target the frame is drawn into. */
        static Framebuffer& getSceneTarget();

        static unsigned int getDrawCallCount();
        static unsigned int getQuadCount();

    private:
        static bool isRendering;
        static glm::vec4 clearColor;
    };
}
