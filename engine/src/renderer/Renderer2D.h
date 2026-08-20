#pragma once

#include <glm/glm.hpp>

namespace ScrapGameEngine
{
    class Texture2D;

    /**
     * @struct Renderer2DStats
     * @brief Per-frame counters, reset by beginScene.
     */
    struct Renderer2DStats
    {
        unsigned int drawCalls = 0;
        unsigned int quadCount = 0;

        unsigned int vertexCount() const { return quadCount * 4; }
        unsigned int indexCount() const { return quadCount * 6; }
    };

    /**
     * @class Renderer2D
     * @brief Batched sprite renderer.
     *
     * Quads accumulate into one dynamic vertex buffer and their textures into a slot
     * array, so a scene full of sprites costs one draw call per batch rather than one
     * per sprite. A batch flushes when it fills up, when it runs out of texture slots,
     * or at endScene.
     *
     * This replaces the fixed-function path, which issued a glDrawArrays per sprite and
     * could not run on a core-profile context at all.
     */
    class Renderer2D
    {
    public:
        Renderer2D() = delete;

        static bool init();
        static void shutdown();

        /** @brief Starts a batch. Resets stats and the vertex cursor. */
        static void beginScene(const glm::mat4& viewProjection);

        /** @brief Flushes whatever is pending. */
        static void endScene();

        /** @brief Draws an axis-aligned quad. */
        static void drawQuad(const glm::vec3& position, const glm::vec2& size,
                             const glm::vec4& color);

        /** @brief Draws a textured, tinted quad. A null texture draws flat colour. */
        static void drawQuad(const glm::vec3& position, const glm::vec2& size,
                             Texture2D* texture, const glm::vec4& tint);

        /** @brief Draws a rotated quad. Rotation is in degrees about Z. */
        static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size,
                                    float rotationDegrees, Texture2D* texture,
                                    const glm::vec4& tint);

        /**
         * @brief Draws a quad from a full transform matrix.
         *
         * The general form the others funnel into; also what a scene graph or an
         * editor gizmo hands over directly.
         */
        static void drawQuad(const glm::mat4& transform, Texture2D* texture,
                             const glm::vec4& tint, int entityId = -1);

        static const Renderer2DStats& getStats();

    private:
        static void startBatch();
        static void flush();
        static float resolveTextureSlot(Texture2D* texture);
    };
}
