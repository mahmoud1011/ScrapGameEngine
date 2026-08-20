#pragma once

namespace ScrapGameEngine
{
    /**
     * @struct FramebufferSpec
     * @brief Size and attachment configuration for a Framebuffer.
     */
    struct FramebufferSpec
    {
        unsigned int width = 1;
        unsigned int height = 1;
        bool depth = true;

        /**
         * @brief Adds an integer attachment holding the entity id per pixel.
         *
         * How viewport picking works: the renderer writes each entity's id alongside
         * its colour, and a click reads back the one pixel under the cursor. No CPU-side
         * ray cast, no per-object bounds test, and it is correct for any geometry the
         * GPU can rasterise - including alpha-cut sprites, which a bounds test gets wrong.
         *
         * Off for the game view, which never needs it and would pay a clear per frame.
         */
        bool entityId = false;
    };

    /**
     * @class Framebuffer
     * @brief An off-screen render target.
     *
     * The renderer draws into one of these rather than the backbuffer, which is what
     * lets the editor show the scene and the running game side by side in two panels:
     * each is just a colour texture an ImGui image can sample.
     */
    class Framebuffer
    {
    public:
        Framebuffer() = default;
        ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        bool create(const FramebufferSpec& spec);

        /**
         * @brief Resizes the target, recreating attachments.
         *
         * A no-op when the size is unchanged, so it is safe to call every frame from a
         * viewport panel. Degenerate and absurd sizes are rejected - a collapsed panel
         * reports 0x0 and would otherwise produce an incomplete framebuffer.
         */
        void resize(unsigned int width, unsigned int height);

        void bind() const;
        static void unbind();

        /** @brief Clears colour and depth, and resets the id attachment to -1. */
        void clear(float r, float g, float b, float a) const;

        /**
         * @brief Reads the entity id at a pixel, or -1 where nothing was drawn.
         *
         * A blocking read of a single pixel. Cheap because it is one pixel and only on
         * click - reading every frame would stall the pipeline waiting on the GPU.
         */
        int readEntityId(int x, int y) const;

        unsigned int getColorAttachment() const { return colorAttachment; }
        unsigned int getEntityIdAttachment() const { return entityIdAttachment; }
        unsigned int getID() const { return id; }
        bool isValid() const { return id != 0; }
        const FramebufferSpec& getSpec() const { return spec; }

    private:
        void destroy();

        unsigned int id = 0;
        unsigned int colorAttachment = 0;
        unsigned int entityIdAttachment = 0;
        unsigned int depthAttachment = 0;
        FramebufferSpec spec;
    };
}
