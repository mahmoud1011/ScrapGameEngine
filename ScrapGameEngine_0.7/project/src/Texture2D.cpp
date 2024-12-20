#pragma once
#include <glad/glad.h>
#include "Texture2D.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

using namespace ScrapGameEngine;

// Utility function to convert TextureWrapMode to OpenGL 
static GLint toGLFormat_WrapMode(TextureWrapMode wrapMode)
{
    switch (wrapMode)
    {
    case TextureWrapMode::REPEAT: return GL_REPEAT;
    case TextureWrapMode::CLAMP: return GL_CLAMP_TO_EDGE;
    case TextureWrapMode::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
    case TextureWrapMode::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
    }
    return GL_REPEAT; // Default to REPEAT if no match
}

// Utility function to convert TextureFilterMode to OpenGL format (Mag Filter)
static GLint toGLFormat_MagFilter(TextureFilterMode filterMode)
{
    switch (filterMode)
    {
    case TextureFilterMode::LINEAR: return GL_LINEAR;
    case TextureFilterMode::NEAREST: return GL_NEAREST; // Handle NEAREST filter mode
    }
    return 0;
}

// Utility function to convert TextureFilterMode to OpenGL format (Min Filter)
static GLint toGLFormat_MinFilter(TextureFilterMode filterMode)
{
    switch (filterMode)
    {
    case TextureFilterMode::LINEAR: return GL_LINEAR;
    case TextureFilterMode::NEAREST: return GL_NEAREST; // Handle NEAREST filter mode
    case TextureFilterMode::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
    case TextureFilterMode::NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
    case TextureFilterMode::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
    }
    return 0;
}

// Utility function to convert TextureFilterMode to OpenGL format (Min and Mag Filters)
static GLint toGLFormat_FilterMode(TextureFilterMode filterMode, bool isMinFilter = true)
{
    switch (filterMode)
    {
    case TextureFilterMode::LINEAR:
        return isMinFilter ? GL_LINEAR : GL_LINEAR;
    case TextureFilterMode::NEAREST:
        return isMinFilter ? GL_NEAREST : GL_NEAREST;
    case TextureFilterMode::NEAREST_MIPMAP_NEAREST:
        return isMinFilter ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
    case TextureFilterMode::NEAREST_MIPMAP_LINEAR:
        return isMinFilter ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;
    case TextureFilterMode::LINEAR_MIPMAP_LINEAR:
        return isMinFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
    }
    return 0;
}

// Helper function to set texture parameters
void Texture2D::setTextureParams(GLuint textureID, const TextureConfig& cfg)
{
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLFormat_FilterMode(cfg.filterMode, true));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLFormat_FilterMode(cfg.filterMode, false));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLFormat_WrapMode(cfg.wrapModeX));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLFormat_WrapMode(cfg.wrapModeY));

    if (cfg.wrapModeX == TextureWrapMode::CLAMP_TO_BORDER || cfg.wrapModeY == TextureWrapMode::CLAMP_TO_BORDER)
    {
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &cfg.borderColor[0]);
    }

    glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture
}

// Unified function to load texture data
static unsigned int loadTexture(const std::string& path, int& width, int& height, int& nrChannels, const TextureConfig& cfg)
{
    stbi_set_flip_vertically_on_load(true); // Flip image vertically as OpenGL expects it this way

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        unsigned int textureID;
        glGenTextures(1, &textureID);  // Generate a new texture ID
        glBindTexture(GL_TEXTURE_2D, textureID);  // Bind the texture for setting parameters

        // Set texture filtering and wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLFormat_FilterMode(cfg.filterMode, true));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLFormat_FilterMode(cfg.filterMode, false));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLFormat_WrapMode(cfg.wrapModeX));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLFormat_WrapMode(cfg.wrapModeY));

        if (cfg.wrapModeX == TextureWrapMode::CLAMP_TO_BORDER || cfg.wrapModeY == TextureWrapMode::CLAMP_TO_BORDER)
        {
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &cfg.borderColor[0]);
        }

        // Load the image data into OpenGL
        GLenum format = (nrChannels == 4) ? GL_RGBA : (nrChannels == 3) ? GL_RGB : GL_RED;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        if (cfg.generateMipmaps) 
        {
            //glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture
        stbi_image_free(data);  // Free the image data from memory
        return textureID;  // Return the generated texture ID
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;  // Return 0 if loading failed
    }
}

// Get the OpenGL texture ID
unsigned int Texture2D::getID() const
{
    return id;
}

// Get the size of the texture
glm::ivec2 Texture2D::getSize() const
{
    return glm::ivec2(width, height);
}

// Get the path of the texture
const std::string& Texture2D::getPath() const
{
    return path;
}

// Get the configuration of the texture
const TextureConfig& Texture2D::getConfig() const
{
    return cfg;
}

// Method to bind the texture
void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::setWrapMode(TextureWrapMode wrapX, TextureWrapMode wrapY)
{
    cfg.wrapModeX = wrapX;
    cfg.wrapModeY = wrapY;

    // Bind the texture and update the wrap parameters
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLFormat_WrapMode(wrapX));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLFormat_WrapMode(wrapY));
}

// Initialize the static blank texture pointer
Texture2D* Texture2D::blankTexture()
{
    static Texture2D* blankTexture = nullptr;

    if (!blankTexture)
    {
        TextureConfig blankConfig = { TextureWrapMode::CLAMP_TO_BORDER, TextureWrapMode::CLAMP_TO_BORDER, TextureFilterMode::NEAREST };
        blankTexture = new Texture2D("Blank", blankConfig);
    }

    return blankTexture;
}

Texture2D* Texture2D::createTexture(const std::string& path, const TextureConfig& cfg)
{
    int width, height, nrChannels;
    unsigned int textureID = loadTexture(path, width, height, nrChannels, cfg);  // Pass cfg to loadTexture

    if (textureID == 0) return nullptr;  // If texture load failed, return nullptr

    // Create the texture object and initialize with the generated texture ID and configuration
    Texture2D* tex = new Texture2D(path, cfg);
    tex->id = textureID;  // Set the ID generated by OpenGL
    tex->width = width;   // Set the width from the loaded image data
    tex->height = height; // Set the height from the loaded image data

    return tex;  // Return created texture object
}

// Destructor: Clean up the texture from GPU memory
Texture2D::~Texture2D()
{
    glDeleteTextures(1, &id);
}

// Constructor: Load texture data from file and upload it to the GPU
Texture2D::Texture2D(const std::string& path, TextureConfig cfg)
    : path(path), cfg(cfg), id(0), width(0), height(0) // Initialize member variables
{
    int nrChannels;
    id = loadTexture(path, width, height, nrChannels, cfg);  // Pass cfg to loadTexture

    if (id > 0) {
        // Successfully loaded, set texture parameters
        setTextureParams(id, cfg);
    }
}
