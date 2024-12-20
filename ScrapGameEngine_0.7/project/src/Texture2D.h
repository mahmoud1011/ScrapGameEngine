#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include <glad/glad.h>

namespace ScrapGameEngine
{
    /**
     * @enum TextureWrapMode
     * @brief Specifies the wrapping mode for a texture.
     */
    enum class TextureWrapMode
    {
        REPEAT,             /**< Repeats the texture. */
        CLAMP,              /**< Clamps the texture to the edge. */
        MIRRORED_REPEAT,    /**< Mirrors the texture when repeating. */
        CLAMP_TO_BORDER     /**< Clamps the texture and uses a border color. */
    };

    /**
     * @enum TextureFilterMode
     * @brief Specifies the filtering mode for a texture.
     */
    enum class TextureFilterMode
    {
        LINEAR,                 /**< Linear filtering for smooth textures. */
        NEAREST,                /**< Nearest-neighbor filtering for pixelated textures. */
        NEAREST_MIPMAP_NEAREST, /**< Nearest filtering with mipmaps. */
        LINEAR_MIPMAP_LINEAR,   /**< Linear filtering with mipmaps. */
        NEAREST_MIPMAP_LINEAR   /**< Nearest texture filtering with linear mipmap blending. */
    };

    /**
     * @struct TextureConfig
     * @brief Configuration options for a texture.
     */
    struct TextureConfig
    {
        TextureWrapMode wrapModeX = TextureWrapMode::REPEAT; /**< Wrap mode for the X-axis. */
        TextureWrapMode wrapModeY = TextureWrapMode::REPEAT; /**< Wrap mode for the Y-axis. */
        TextureFilterMode filterMode = TextureFilterMode::LINEAR; /**< Texture filtering mode. */
        glm::vec4 borderColor = glm::vec4(0.0f); /**< Border color used when `CLAMP_TO_BORDER` is set. */
        bool generateMipmaps = false; /**< Indicates if mipmaps should be generated. */
    };

    /**
     * @class Texture2D
     * @brief Represents a 2D texture in the game engine.
     *
     * This class provides functionality for creating, configuring, and managing 2D textures.
     */
    class Texture2D
    {
    public:
        /**
         * @brief Returns a blank texture.
         * @return Pointer to a blank `Texture2D`.
         */
        static Texture2D* blankTexture();

        /**
         * @brief Creates a texture from a given file path and configuration.
         * @param path The path to the texture file.
         * @param cfg The configuration for the texture.
         * @return Pointer to the created `Texture2D`.
         */
        static Texture2D* createTexture(const std::string& path, const TextureConfig& cfg);

        /**
         * @brief Sets the parameters for a texture.
         * @param textureID The ID of the texture.
         * @param cfg The configuration to apply.
         */
        static void setTextureParams(GLuint textureID, const TextureConfig& cfg);

        /**
         * @brief Retrieves the OpenGL texture ID.
         * @return The OpenGL texture ID.
         */
        unsigned int getID() const;

        /**
         * @brief Retrieves the size of the texture.
         * @return A `glm::ivec2` representing the width and height of the texture.
         */
        glm::ivec2 getSize() const;

        /**
         * @brief Retrieves the path of the texture file.
         * @return The file path as a string.
         */
        const std::string& getPath() const;

        /**
         * @brief Retrieves the texture configuration.
         * @return A reference to the `TextureConfig`.
         */
        const TextureConfig& getConfig() const;

        /**
         * @brief Binds the texture to the current OpenGL context.
         */
        void bind() const;

        /**
         * @brief Sets the wrapping mode for the texture.
         * @param wrapX The wrap mode for the X-axis.
         * @param wrapY The wrap mode for the Y-axis.
         */
        void setWrapMode(TextureWrapMode wrapX, TextureWrapMode wrapY);

        /**
         * @brief Destructor for the `Texture2D` class.
         */
        ~Texture2D();

        /**
         * @brief Constructs a `Texture2D` with a file path and configuration.
         * @param path The path to the texture file.
         * @param cfg The configuration for the texture.
         */
        Texture2D(const std::string& path, TextureConfig cfg);

    private:
        TextureConfig cfg; /**< The configuration of the texture. */
        std::string path; /**< The file path of the texture. */
        unsigned int id; /**< The OpenGL texture ID. */
        int width; /**< The width of the texture in pixels. */
        int height; /**< The height of the texture in pixels. */
    };
}
