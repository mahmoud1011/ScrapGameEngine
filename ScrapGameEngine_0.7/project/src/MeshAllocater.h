#pragma once
#include "Mesh.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace ScrapGameEngine
{
    /**
     * @class MeshAllocator
     * @brief Manages the allocation and caching of Mesh objects.
     *
     * The MeshAllocator class provides static methods to retrieve and cache Mesh objects.
     * It allows efficient sharing of Mesh resources across different parts of the application.
     * Meshes are stored in a cache, identified by a unique key derived from their vertex data.
     */
    class MeshAllocator
    {
    public:
        /**
         * @brief Retrieve a Mesh with the given ID or create it if it doesn't exist.
         *
         * This method either fetches a Mesh from the cache if it has already been created
         * with the same vertex data, or it creates a new Mesh, adds it to the cache, and returns it.
         *
         * @param vertices Vector of vertices defining the Mesh.
         * @return Pointer to the Mesh.
         */
        static Mesh* getMesh(const std::vector<Vertex>& vertices);

        /**
         * @brief Release all unused meshes and clear the cache.
         *
         * This method clears out all Meshes from the cache that are not currently in use.
         * It is useful for freeing memory and optimizing resource usage.
         */
        static void releaseUnusedMeshes();

    private:
        static std::unordered_map<size_t, std::shared_ptr<Mesh>> meshCache; /**< Cache for storing Mesh objects. */
    };
}
