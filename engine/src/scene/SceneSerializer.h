#pragma once

#include <memory>
#include <string>

namespace Scrap
{
    class Scene;
    class Entity;

    /**
     * @class SceneSerializer
     * @brief Reads and writes scenes as YAML.
     *
     * D5. YAML rather than JSON because scene files get committed and merged, and a
     * YAML diff stays readable where JSON's braces and quotes do not.
     */
    class SceneSerializer
    {
    public:
        explicit SceneSerializer(std::shared_ptr<Scene> scene) : scene(std::move(scene)) {}

        /** @brief Writes the scene to a .scrapscene file. */
        bool serialize(const std::string& path);

        /**
         * @brief Loads a .scrapscene file into the scene, replacing its contents.
         * @return False on a missing or malformed file; the scene is left untouched.
         */
        bool deserialize(const std::string& path);

        // --- prefabs ---------------------------------------------------------
        // A prefab is one entity written with the same per-component code a scene
        // uses, so a component supported in a scene is automatically supported in a
        // prefab - there is no second format to keep in step.

        /** @brief Writes a single entity to a .scrapprefab file. */
        static bool savePrefab(Entity entity, const std::string& path);

        /**
         * @brief Instantiates a prefab into a scene.
         * @return The new entity, or an invalid Entity if the file could not be read.
         *
         * The instance gets a fresh UUID rather than the one in the file: two copies
         * of the same prefab in one scene must not collide, and reusing the file's id
         * would make the second displace the first in the lookup table.
         */
        static Entity instantiatePrefab(Scene& scene, const std::string& path);

    private:
        std::shared_ptr<Scene> scene;
    };
}
