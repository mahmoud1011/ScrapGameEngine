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
    };

    /**
     * @class Framebuffer
     * @brief An off-screen render target with a colour texture and optional depth.
     *
     * The renderer draws into one of these rather than straight to the backbuffer,
     * which is what lets the editor display the running game inside a panel: the colour
     * attachment is just a texture id an ImGui image can sample. The runtime blits it
     * to the window instead, and pays nothing for the indirection.
     */
    class Framebuffer
    {
    public:
        Framebuffer() = default;
        ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        /** @brief Builds (or rebuilds) the framebuffer and its attachments. */
        bool create(const FramebufferSpec& spec);

        /**
         * @brief Resizes the target, recreating attachments.
         *
         * A no-op when the size is unchanged, so it is safe to call every frame from a
         * viewport panel. Zero or absurd sizes are rejected - a minimised editor panel
         * reports 0x0 and would otherwise produce an incomplete framebuffer.
         */
        void resize(unsigned int width, unsigned int height);

        void bind() const;
        static void unbind();

        unsigned int getColorAttachment() const { return colorAttachment; }
        unsigned int getID() const { return id; }
        bool isValid() const { return id != 0; }
        const FramebufferSpec& getSpec() const { return spec; }

    private:
        void destroy();

        unsigned int id = 0;
        unsigned int colorAttachment = 0;
        unsigned int depthAttachment = 0;
        FramebufferSpec spec;
    };
}
