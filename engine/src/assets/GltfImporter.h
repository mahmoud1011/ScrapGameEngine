#pragma once

#include <glm/glm.hpp>

#include "renderer/Mesh3D.h"

#include <memory>
#include <string>
#include <vector>

namespace ScrapGameEngine
{
    class Mesh3D;

    /**
     * @struct ImportedPrimitive
     * @brief One drawable piece of an imported model, with its material.
     *
     * A glTF mesh can hold several primitives with different materials, and a file can
     * hold several meshes. Flattening to a list of primitives keeps the importer's
     * output shaped like what the renderer actually draws.
     */
    struct ImportedPrimitive
    {
        // CPU-side geometry. Parsing deliberately produces this rather than a GPU
        // mesh: it needs no GL context, so it is testable headlessly and can move to
        // a worker thread when the asset pipeline lands.
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;

        std::string name;

        glm::vec4 albedo{1.0f};
        float metallic = 0.0f;
        float roughness = 0.5f;
        float emissive = 0.0f;

        glm::mat4 transform{1.0f};   ///< The node transform this primitive sits under.

        /**
         * @brief Uploads the geometry and returns a drawable mesh.
         *
         * Must be called with a current GL context - so, on the render thread.
         */
        std::shared_ptr<Mesh3D> upload() const;

        size_t triangleCount() const { return indices.size() / 3; }
    };

    /**
     * @class GltfImporter
     * @brief Loads glTF 2.0 and .glb into engine meshes and materials.
     *
     * glTF rather than FBX or OBJ: it is the format with a specification rather than a
     * reverse-engineered one, its material model is metallic-roughness (exactly what
     * Renderer3D already implements, so nothing is converted), and it needs no
     * heavyweight SDK.
     */
    class GltfImporter
    {
    public:
        GltfImporter() = delete;

        /**
         * @brief Loads every primitive in a .gltf or .glb file.
         * @return An empty vector when the file cannot be read; the reason is logged.
         *
         * Node transforms are baked into each primitive's transform, so a hierarchy
         * imports at the right place without the scene needing parenting first.
         */
        static std::vector<ImportedPrimitive> load(const std::string& path);

        /** @brief True when the extension is one this importer handles. */
        static bool isSupported(const std::string& path);
    };
}
