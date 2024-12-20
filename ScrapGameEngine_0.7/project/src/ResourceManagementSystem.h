#pragma once
#include "IResourceManager.h"
#include "ResourceAllocator.h"
#include "Texture2D.h"
#include "Mesh.h"
#include "GameObject.h"
#include <memory>
#include <mutex>

namespace ScrapGameEngine
{
    class ResourceManagementSystem : public IResourceManager
    {
    public:
        static ResourceManagementSystem& getInstance();

        // Template method for loading resources
        template <typename ResourceType, typename... Args>
        ResourceType* loadResource(const std::string& path, Args&&... args)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return getAllocator<ResourceType>().getResource(path, std::forward<Args>(args)...);
        }

        void releaseResource(const std::string& path) override;
        void garbageCollect() override;

    private:
        ResourceManagementSystem() = default;
        std::mutex mutex_;

        ResourceAllocator<Texture2D> textureAllocator;
        ResourceAllocator<Mesh> meshAllocator;
        ResourceAllocator<GameObject> gameObjectAllocator;

        // Template specialization to retrieve the appropriate allocator
        template <typename ResourceType>
        ResourceAllocator<ResourceType>& getAllocator();
    };
}
