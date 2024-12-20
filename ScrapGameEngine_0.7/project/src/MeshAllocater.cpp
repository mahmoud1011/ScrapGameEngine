#include "MeshAllocater.h"
#include <functional>  // For std::hash
#include <iostream>    // For std::cout

namespace ScrapGameEngine
{
    /**
    * Static _mesh cache to store and reuse meshes with unique vertex data.
    */    
    std::unordered_map<size_t, std::shared_ptr<Mesh>> MeshAllocator::meshCache;

    /**
    * Generates a hash for the _mesh's vertex data.
    *
    * This function uses the XOR operator to combine the hashes of each vertex's
    * x, y, z, u, and v components. This produces a unique hash for each unique
    * set of vertex data.
    *
    * @param vertices The vector of vertices to generate a hash for.
    * @return A size_t hash value representing the vertex data.
    */
    size_t hashVertices(const std::vector<Vertex>& vertices)
    {
        size_t hash = 0;
        for (const auto& vertex : vertices)
        {
            hash ^= std::hash<float>{}(vertex.x) ^ std::hash<float>{}(vertex.y) ^ std::hash<float>{}(vertex.z);
            hash ^= std::hash<float>{}(vertex.u) ^ std::hash<float>{}(vertex.v);
        }
        return hash;
    }

    /**
    * Retrieves a _mesh from the cache based on the provided vertex data.
    *
    * If a _mesh with the same vertex data is already cached, it is returned.
    * Otherwise, a new _mesh is created, cached, and returned.
    *
    * @param vertices The vertex data to retrieve a _mesh for.
    * @return A pointer to the _mesh.
    */
    Mesh* MeshAllocator::getMesh(const std::vector<Vertex>& vertices)
    {
        size_t hash = hashVertices(vertices);

        // Check if the _mesh is already created and cached
        auto it = meshCache.find(hash);
        if (it != meshCache.end())
        {
            std::cout << "[ALLOCATER] Reusing and caching _mesh with hash: " << hash << std::endl;
            // Return the cached _mesh if found
            return it->second.get();
        }
        else
        {
            std::cout << "[ALLOCATER] Loaded and added _mesh with hash: " << hash << std::endl;
            // Create a new _mesh and cache it
            auto newMesh = std::make_unique<Mesh>(vertices);
            auto meshPtr = newMesh.get();
            meshCache[hash] = std::move(newMesh);
            return meshPtr;
        }
    }

    // Releases all unused meshes and clears the cache
    void MeshAllocator::releaseUnusedMeshes()
    {
        std::cout << "[ALLOCATER] Releasing unused meshes..." << std::endl;
        meshCache.clear();  // Clear the _mesh cache
    }
}
