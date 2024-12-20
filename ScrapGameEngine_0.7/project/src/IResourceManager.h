#pragma once
#include <string>

namespace ScrapGameEngine
{
    class IResourceManager
    {
    public:
        virtual ~IResourceManager() = default;

        // Retrieve or load a resource of any type
        template <typename ResourceType, typename... Args>
        ResourceType* loadResource(const std::string& path, Args&&... args);

        // Release a resource by path
        virtual void releaseResource(const std::string& path) = 0;

        // Perform global garbage collection
        virtual void garbageCollect() = 0;

    protected:
        IResourceManager() = default;
    };

    template <typename ResourceType, typename... Args>
    ResourceType* IResourceManager::loadResource(const std::string& path, Args&&... args)
    {
        // We delegate this to the derived class implementation
        return static_cast<ResourceType*>(nullptr);
    }
}
