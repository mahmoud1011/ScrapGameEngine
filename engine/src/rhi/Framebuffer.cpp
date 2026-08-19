#include "rhi/Framebuffer.h"

#include <glad/glad.h>
#include <iostream>

using namespace ScrapGameEngine;

namespace
{
    // Guards against a minimised or collapsed viewport panel reporting a degenerate
    // size, and against a runaway resize allocating gigabytes of attachments.
    constexpr unsigned int kMaxDimension = 8192;
}

Framebuffer::~Framebuffer()
{
    destroy();
}

void Framebuffer::destroy()
{
    if (colorAttachment != 0) glDeleteTextures(1, &colorAttachment);
    if (depthAttachment != 0) glDeleteTextures(1, &depthAttachment);
    if (id != 0) glDeleteFramebuffers(1, &id);
    colorAttachment = depthAttachment = id = 0;
}

bool Framebuffer::create(const FramebufferSpec& newSpec)
{
    if (newSpec.width == 0 || newSpec.height == 0 ||
        newSpec.width > kMaxDimension || newSpec.height > kMaxDimension)
    {
        std::cerr << "[FBO] refusing size " << newSpec.width << "x" << newSpec.height
                  << " (must be within 1.." << kMaxDimension << ")." << std::endl;
        return false;
    }

    destroy();
    spec = newSpec;

    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    glGenTextures(1, &colorAttachment);
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 static_cast<GLsizei>(spec.width), static_cast<GLsizei>(spec.height),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

    if (spec.depth)
    {
        glGenTextures(1, &depthAttachment);
        glBindTexture(GL_TEXTURE_2D, depthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
                     static_cast<GLsizei>(spec.width), static_cast<GLsizei>(spec.height),
                     0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
    }

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "[FBO] incomplete (status 0x" << std::hex << status << std::dec << ")." << std::endl;
        destroy();
        return false;
    }
    return true;
}

void Framebuffer::resize(unsigned int width, unsigned int height)
{
    if (width == spec.width && height == spec.height) return;

    FramebufferSpec next = spec;
    next.width = width;
    next.height = height;
    create(next);
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, static_cast<GLsizei>(spec.width), static_cast<GLsizei>(spec.height));
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
