#pragma once
#include "Texture2D.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <mutex>

namespace ScrapGameEngine
{
    /**
     * @struct AllocatedTexture
     * @brief Stores the actual texture and its reference count.
     *
     * This structure encapsulates a `Texture2D` pointer and a reference count, which helps in
     * managing the lifecycle of textures efficiently. The reference count indicates how many
     * times the texture is being used.
     */
    struct AllocatedTexture
    {
        unsigned int refCount; ///< Number of references to the texture
        Texture2D* texture;    ///< Pointer to the actual texture

        /**
         * @brief Default constructor initializing a nullptr texture and zero reference count.
         */
        AllocatedTexture() : texture(nullptr), refCount(0) {}

        /**
         * @brief Parameterized constructor for an `AllocatedTexture` with a given `Texture2D`.
         * @param t Pointer to the `Texture2D`.
         */
        AllocatedTexture(Texture2D* t) : texture(t), refCount(0) {}
    };

    /**
     * @class TextureAllocator
     * @brief Manages the loading, retrieval, and release of textures.
     *
     * The `TextureAllocator` is responsible for handling all aspects of texture management
     * within the game, including loading textures from disk, caching them, and releasing
     * textures when they are no longer needed. It ensures that texture resources are
     * efficiently used and released.
     */
    class TextureAllocator
    {
    public:
        TextureAllocator() = delete; ///< Prevent instantiation of this class.

        /**
         * @brief Retrieves a texture by its path.
         *
         * If the texture is not already loaded, it will be loaded using the provided
         * `TextureConfig`. The texture is then added to the `textureCache`.
         *
         * @param texturePath The path to the texture file.
         * @param cfg Configuration settings for loading the texture.
         * @return Pointer to the `Texture2D` if found or loaded.
         */
        static Texture2D* getTexture(std::string& texturePath, TextureConfig& cfg);

        /**
         * @brief Returns a texture, decrementing its reference count.
         *
         * If the reference count drops to zero, the texture is released.
         *
         * @param texture Pointer to the `Texture2D` to return.
         */
        static void returnTexture(Texture2D* texture);

        /**
         * @brief Releases all textures that are no longer in use (reference count is zero).
         */
        static void releaseUnusedTextures();

    private:
        /**
         * @brief Collection to store all allocated textures indexed by their path.
         */
        static std::unordered_map<std::string, AllocatedTexture> textureCache;

        /**
         * @brief Mutex to ensure thread-safe operations on the texture cache.
         */
        static std::mutex cacheMutex;
    };
}
