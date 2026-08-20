#include "rhi/Framebuffer.h"

#include <glad/glad.h>
#include <iostream>

using namespace ScrapGameEngine;

namespace
{
    // Guards a collapsed panel reporting a degenerate size, and a runaway resize
    // allocating gigabytes of attachments.
    constexpr unsigned int kMaxDimension = 8192;
}

Framebuffer::~Framebuffer()
{
    destroy();
}

void Framebuffer::destroy()
{
    if (colorAttachment != 0) glDeleteTextures(1, &colorAttachment);
    if (entityIdAttachment != 0) glDeleteTextures(1, &entityIdAttachment);
    if (depthAttachment != 0) glDeleteTextures(1, &depthAttachment);
    if (id != 0) glDeleteFramebuffers(1, &id);
    colorAttachment = entityIdAttachment = depthAttachment = id = 0;
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

    const auto w = static_cast<GLsizei>(spec.width);
    const auto h = static_cast<GLsizei>(spec.height);

    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    glGenTextures(1, &colorAttachment);
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

    if (spec.entityId)
    {
        // R32I: an entity index is an integer, and sampling filters would corrupt it.
        glGenTextures(1, &entityIdAttachment);
        glBindTexture(GL_TEXTURE_2D, entityIdAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, w, h, 0, GL_RED_INTEGER, GL_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, entityIdAttachment, 0);

        const GLenum targets[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, targets);
    }
    else
    {
        const GLenum target = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &target);
    }

    if (spec.depth)
    {
        glGenTextures(1, &depthAttachment);
        glBindTexture(GL_TEXTURE_2D, depthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0,
                     GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
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

void Framebuffer::clear(float r, float g, float b, float a) const
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // -1 rather than 0, because 0 is a valid entity index and would read as a hit.
    if (spec.entityId)
    {
        const int clearValue = -1;
        glClearBufferiv(GL_COLOR, 1, &clearValue);
    }
}

int Framebuffer::readEntityId(int x, int y) const
{
    if (!spec.entityId || id == 0) return -1;
    if (x < 0 || y < 0 ||
        x >= static_cast<int>(spec.width) || y >= static_cast<int>(spec.height))
    {
        return -1;
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
    glReadBuffer(GL_COLOR_ATTACHMENT1);

    int value = -1;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &value);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    return value;
}
