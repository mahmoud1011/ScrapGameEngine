#pragma once

#include <memory>
#include <string>

namespace Scrap
{
    class Scene;

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

    private:
        std::shared_ptr<Scene> scene;
    };
}
