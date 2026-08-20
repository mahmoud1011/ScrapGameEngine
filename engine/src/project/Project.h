#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Scrap
{
    /**
     * @struct ProjectConfig
     * @brief What a .scrapproject file stores.
     */
    struct ProjectConfig
    {
        std::string name = "Untitled";
        std::string startScene;       ///< Relative to assets/, e.g. "scenes/Main.scrapscene"
        std::string engineVersion;
    };

    /**
     * @class Project
     * @brief An open project: its root directory and the layout beneath it.
     *
     * A project is a folder with a `.scrapproject` file at the top, the same shape Unity
     * and Unreal use. Everything the editor opens, saves and browses is resolved against
     * it, which is what replaces the hardcoded "assets/scenes/Sample.scrapscene" the
     * editor previously assumed.
     *
     * Paths are stored absolute internally and exposed relative where a user sees them,
     * so a project stays portable when the folder moves.
     */
    class Project
    {
    public:
        static constexpr const char* kFileExtension = ".scrapproject";
        static constexpr const char* kSceneExtension = ".scrapscene";
        static constexpr const char* kPrefabExtension = ".scrapprefab";

        /**
         * @brief Creates a new project on disk, with the standard folder layout.
         * @return Null when the directory could not be created or is already a project.
         */
        static std::shared_ptr<Project> create(const std::filesystem::path& directory,
                                               const std::string& name);

        /**
         * @brief Opens an existing project.
         * @param path Either the .scrapproject file or the directory containing it.
         */
        static std::shared_ptr<Project> open(const std::filesystem::path& path);

        /** @brief The project currently open, or null. */
        static std::shared_ptr<Project> active();
        static void setActive(std::shared_ptr<Project> project);

        bool save() const;

        const ProjectConfig& getConfig() const { return config; }
        ProjectConfig& getConfig() { return config; }

        const std::filesystem::path& root() const { return rootPath; }
        std::filesystem::path assetsDirectory() const { return rootPath / "assets"; }
        std::filesystem::path scenesDirectory() const { return assetsDirectory() / "scenes"; }
        std::filesystem::path scriptsDirectory() const { return assetsDirectory() / "scripts"; }
        std::filesystem::path prefabsDirectory() const { return assetsDirectory() / "prefabs"; }
        std::filesystem::path modelsDirectory() const { return assetsDirectory() / "models"; }

        /** @brief Absolute path for something stored relative to assets/. */
        std::filesystem::path resolve(const std::string& relativeToAssets) const;

        /** @brief The assets-relative form of an absolute path, for storing in files. */
        std::string relativize(const std::filesystem::path& absolute) const;

        /** @brief Every .scrapscene under assets/, for the hub and the Open menu. */
        std::vector<std::filesystem::path> findScenes() const;

        /** @brief Every .cs file under assets/. */
        std::vector<std::filesystem::path> findScripts() const;

    private:
        std::filesystem::path rootPath;
        ProjectConfig config;
    };

    /**
     * @struct RecentProject
     * @brief One entry in the hub's recent list.
     */
    struct RecentProject
    {
        std::string name;
        std::string path;
        bool exists = true;     ///< False when the folder has since been moved or deleted.
    };

    /**
     * @class ProjectRegistry
     * @brief The list of recently opened projects, stored beside the editor.
     *
     * Kept out of Project itself: it is editor state about projects, not state
     * belonging to any one of them.
     */
    class ProjectRegistry
    {
    public:
        ProjectRegistry() = delete;

        static const std::vector<RecentProject>& recents();

        /** @brief Records a project as most-recently-used, de-duplicating by path. */
        static void remember(const std::shared_ptr<Project>& project);

        /** @brief Drops an entry, for a project the user no longer wants listed. */
        static void forget(const std::string& path);

        static void load();
        static bool save();
    };
}
