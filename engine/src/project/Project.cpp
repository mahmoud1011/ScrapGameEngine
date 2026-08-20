#include "project/Project.h"

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace Scrap
{
    namespace
    {
        std::shared_ptr<Project> activeProject;
        std::vector<RecentProject> recentProjects;
        bool recentsLoaded = false;

        /** The editor's own settings live beside the executable, not in any project. */
        fs::path recentsFile() { return "scrap-recent-projects.yaml"; }

        /** A starter script, so a new project has something that compiles. */
        constexpr const char* kStarterScript = R"(using Scrap;

namespace Game;

/// <summary>Attach by putting Game.Player into a ScriptComponent's Type field.</summary>
public sealed class Player : ScriptableEntity
{
    public float Speed { get; set; } = 2.0f;

    private float _elapsed;

    public override void OnCreate()
    {
        Log.Info($"Player ready at {Entity.Position}");
    }

    public override void OnUpdate(float deltaTime)
    {
        _elapsed += deltaTime;
        Entity.Position = Entity.Position with
        {
            Y = MathF.Sin(_elapsed * Speed) * 0.5f,
        };
    }
}
)";
    }

    std::shared_ptr<Project> Project::create(const fs::path& directory, const std::string& name)
    {
        std::error_code ec;

        if (fs::exists(directory) && !fs::is_empty(directory, ec))
        {
            // Refuse rather than scatter a project layout through someone's folder.
            std::cerr << "[PROJECT] " << directory << " is not empty." << std::endl;
            return nullptr;
        }

        auto project = std::make_shared<Project>();
        project->rootPath = fs::absolute(directory);
        project->config.name = name.empty() ? directory.filename().string() : name;
        project->config.startScene = "scenes/Main.scrapscene";

        for (const auto& dir : {project->assetsDirectory(), project->scenesDirectory(),
                                project->scriptsDirectory(), project->prefabsDirectory(),
                                project->modelsDirectory()})
        {
            fs::create_directories(dir, ec);
            if (ec)
            {
                std::cerr << "[PROJECT] could not create " << dir << ": " << ec.message() << std::endl;
                return nullptr;
            }
        }

        // A starter script means the scripts folder is not an empty mystery, and it
        // documents the API by example.
        std::ofstream script(project->scriptsDirectory() / "Player.cs");
        if (script) script << kStarterScript;

        if (!project->save()) return nullptr;

        std::cout << "[PROJECT] created " << project->config.name
                  << " at " << project->rootPath.string() << std::endl;
        return project;
    }

    std::shared_ptr<Project> Project::open(const fs::path& path)
    {
        std::error_code ec;
        fs::path projectFile = path;

        if (fs::is_directory(path, ec))
        {
            // Accept a folder and find the project file in it, since that is what a
            // user picks in a file dialog.
            projectFile.clear();
            for (const auto& entry : fs::directory_iterator(path, ec))
            {
                if (entry.path().extension() == kFileExtension)
                {
                    projectFile = entry.path();
                    break;
                }
            }
            if (projectFile.empty())
            {
                std::cerr << "[PROJECT] no " << kFileExtension << " in " << path << std::endl;
                return nullptr;
            }
        }

        if (!fs::exists(projectFile, ec))
        {
            std::cerr << "[PROJECT] not found: " << projectFile << std::endl;
            return nullptr;
        }

        YAML::Node data;
        try
        {
            data = YAML::LoadFile(projectFile.string());
        }
        catch (const YAML::Exception& e)
        {
            std::cerr << "[PROJECT] could not parse " << projectFile << ": " << e.what() << std::endl;
            return nullptr;
        }

        auto project = std::make_shared<Project>();
        project->rootPath = fs::absolute(projectFile).parent_path();
        project->config.name = data["Name"] ? data["Name"].as<std::string>()
                                            : project->rootPath.filename().string();
        if (data["StartScene"]) project->config.startScene = data["StartScene"].as<std::string>();
        if (data["EngineVersion"]) project->config.engineVersion = data["EngineVersion"].as<std::string>();

        // Tolerate a project whose folders were partly deleted, rather than failing to
        // open something the user can still fix from inside the editor.
        std::error_code mkdirEc;
        for (const auto& dir : {project->assetsDirectory(), project->scenesDirectory(),
                                project->scriptsDirectory()})
        {
            fs::create_directories(dir, mkdirEc);
        }

        return project;
    }

    bool Project::save() const
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << config.name;
        out << YAML::Key << "StartScene" << YAML::Value << config.startScene;
        out << YAML::Key << "EngineVersion" << YAML::Value
            << (config.engineVersion.empty() ? "0.8.0" : config.engineVersion);
        out << YAML::EndMap;

        const fs::path file = rootPath / (config.name + kFileExtension);
        std::ofstream stream(file);
        if (!stream)
        {
            std::cerr << "[PROJECT] could not write " << file << std::endl;
            return false;
        }
        stream << out.c_str();
        return true;
    }

    std::shared_ptr<Project> Project::active() { return activeProject; }
    void Project::setActive(std::shared_ptr<Project> project) { activeProject = std::move(project); }

    fs::path Project::resolve(const std::string& relativeToAssets) const
    {
        return assetsDirectory() / relativeToAssets;
    }

    std::string Project::relativize(const fs::path& absolute) const
    {
        std::error_code ec;
        const fs::path relative = fs::relative(absolute, assetsDirectory(), ec);
        if (ec || relative.empty() || relative.string().rfind("..", 0) == 0)
        {
            // Outside the project, so there is no meaningful relative form.
            return absolute.string();
        }
        return relative.generic_string();
    }

    std::vector<fs::path> Project::findScenes() const
    {
        std::vector<fs::path> found;
        std::error_code ec;
        if (!fs::exists(assetsDirectory(), ec)) return found;

        for (const auto& entry : fs::recursive_directory_iterator(assetsDirectory(), ec))
        {
            if (entry.is_regular_file(ec) && entry.path().extension() == kSceneExtension)
                found.push_back(entry.path());
        }
        std::sort(found.begin(), found.end());
        return found;
    }

    std::vector<fs::path> Project::findScripts() const
    {
        std::vector<fs::path> found;
        std::error_code ec;
        if (!fs::exists(assetsDirectory(), ec)) return found;

        for (const auto& entry : fs::recursive_directory_iterator(assetsDirectory(), ec))
        {
            if (entry.is_regular_file(ec) && entry.path().extension() == ".cs")
                found.push_back(entry.path());
        }
        std::sort(found.begin(), found.end());
        return found;
    }

    // ------------------------------------------------------------ registry

    const std::vector<RecentProject>& ProjectRegistry::recents()
    {
        if (!recentsLoaded) load();
        return recentProjects;
    }

    void ProjectRegistry::remember(const std::shared_ptr<Project>& project)
    {
        if (!project) return;
        if (!recentsLoaded) load();

        const std::string path = project->root().string();

        // De-duplicate by path and move to the front, so the list is genuinely
        // most-recent-first rather than accumulating duplicates.
        recentProjects.erase(
            std::remove_if(recentProjects.begin(), recentProjects.end(),
                           [&](const RecentProject& r) { return r.path == path; }),
            recentProjects.end());

        recentProjects.insert(recentProjects.begin(),
                              {project->getConfig().name, path, true});

        constexpr size_t kMaxRecents = 12;
        if (recentProjects.size() > kMaxRecents) recentProjects.resize(kMaxRecents);

        save();
    }

    void ProjectRegistry::forget(const std::string& path)
    {
        if (!recentsLoaded) load();
        recentProjects.erase(
            std::remove_if(recentProjects.begin(), recentProjects.end(),
                           [&](const RecentProject& r) { return r.path == path; }),
            recentProjects.end());
        save();
    }

    void ProjectRegistry::load()
    {
        recentsLoaded = true;
        recentProjects.clear();

        std::error_code ec;
        if (!fs::exists(recentsFile(), ec)) return;

        YAML::Node data;
        try
        {
            data = YAML::LoadFile(recentsFile().string());
        }
        catch (const YAML::Exception&)
        {
            // A corrupt recents list is not worth failing over - start empty.
            return;
        }

        const auto list = data["Recent"];
        if (!list) return;

        for (const auto& node : list)
        {
            RecentProject entry;
            entry.name = node["Name"] ? node["Name"].as<std::string>() : "Project";
            entry.path = node["Path"] ? node["Path"].as<std::string>() : "";
            if (entry.path.empty()) continue;
            // Checked at load so the hub can grey out projects that have moved.
            entry.exists = fs::exists(entry.path, ec);
            recentProjects.push_back(entry);
        }
    }

    bool ProjectRegistry::save()
    {
        YAML::Emitter out;
        out << YAML::BeginMap << YAML::Key << "Recent" << YAML::Value << YAML::BeginSeq;
        for (const auto& entry : recentProjects)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << entry.name;
            out << YAML::Key << "Path" << YAML::Value << entry.path;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq << YAML::EndMap;

        std::ofstream stream(recentsFile());
        if (!stream) return false;
        stream << out.c_str();
        return true;
    }
}
