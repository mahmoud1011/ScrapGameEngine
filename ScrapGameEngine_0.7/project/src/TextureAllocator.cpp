#include "TextureAllocator.h"
#include <iostream>
#include <memory>

namespace ScrapGameEngine
{
    // Initialize the static variables outside of the class definition
    std::unordered_map<std::string, AllocatedTexture> TextureAllocator::textureCache;
    std::mutex TextureAllocator::cacheMutex;

    Texture2D* TextureAllocator::getTexture(std::string& texturePath, TextureConfig& cfg)
    {
        std::lock_guard<std::mutex> lock(cacheMutex);

        // Check if texture already exists in cache
        auto it = textureCache.find(texturePath);
        if (it != textureCache.end())
        {
            it->second.refCount++;
            std::cout << "[ALLOCATER] Reusing texture: " << texturePath << " :: refCount: " << it->second.refCount << std::endl;
            return it->second.texture;
        }

        // If texture is not found, load it using Texture2D
        Texture2D* newTexture = new Texture2D(texturePath, cfg);  // Load texture from file
        if (newTexture)
        {
            AllocatedTexture allocated(newTexture);
            allocated.refCount = 1;

            textureCache.emplace(texturePath, allocated);  // Store the new texture in the cache
            newTexture->bind();
            std::cout << "[ALLOCATER] Loaded and added texture: " << texturePath << std::endl;
            return newTexture;
        }

        std::cout << "Faild to load texture: " << std::endl;
        return nullptr;  // Return null if loading fails
    }

    void TextureAllocator::returnTexture(Texture2D* texture)
    {
        std::lock_guard<std::mutex> lock(cacheMutex);

        // Find the texture in cache
        for (auto& entry : textureCache)
        {
            if (entry.second.texture == texture)
            {
                entry.second.refCount--;
                std::cout << "[ALLOCATER] Returned texture: " << entry.first << " :: refCount: " << entry.second.refCount << std::endl;

                // If refCount is 0, remove from cache
                if (entry.second.refCount == 0)
                {
                    std::cout << "[ALLOCATER] Texture: " << entry.first << " refCount is 0! Removing texture from cache." << std::endl;
                    textureCache.erase(entry.first);
                    delete texture;  // Clean up memory
                }
                return;
            }
        }

        std::cerr << "[ALLOCATER] Error: Texture not found in the cache when returning." << std::endl;
    }

    void TextureAllocator::releaseUnusedTextures()
    {
        std::lock_guard<std::mutex> lock(cacheMutex);

        // Iterate through and release textures with refCount == 0
        for (auto it = textureCache.begin(); it != textureCache.end(); )
        {
            if (it->second.refCount == 0)
            {
                std::cout << "[ALLOCATER] Releasing unused texture: " << it->first << std::endl;

                // Return the texture and delete it
                returnTexture(it->second.texture);
                it = textureCache.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}