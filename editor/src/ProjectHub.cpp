#include "ProjectHub.h"
#include "ImGuiLayer.h"
#include "ScrapTheme.h"

#include "project/Project.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace Scrap::Editor
{
    namespace
    {
        using P = Palette;

        char newProjectName[96] = "MyGame";
        char newProjectPath[512] = "";
        char openPath[512] = "";
        std::string statusMessage;
        bool statusIsError = false;

        /** A sensible default location, so the path field is never empty on first run. */
        void ensureDefaultPath()
        {
            if (newProjectPath[0] != '\0') return;
            const fs::path base = fs::absolute("projects");
            std::snprintf(newProjectPath, sizeof(newProjectPath), "%s",
                          base.string().c_str());
        }

        void setStatus(std::string message, bool error)
        {
            statusMessage = std::move(message);
            statusIsError = error;
        }

        /** Section heading, matching the editor's uppercase label treatment. */
        void heading(const char* text)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, P::InkMuted);
            ImGui::SetWindowFontScale(0.85f);
            ImGui::TextUnformatted(text);
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();
            ImGui::Spacing();
        }
    }

    std::shared_ptr<Project> ProjectHub::run()
    {
        // The hub owns a window of its own rather than reusing the editor's, so the
        // editor can be constructed already knowing which project it is editing.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        GLFWwindow* window = glfwCreateWindow(920, 580, "ScrapEngine Hub", nullptr, nullptr);
        if (window == nullptr)
        {
            std::cerr << "[HUB] could not create the hub window." << std::endl;
            return nullptr;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            std::cerr << "[HUB] could not load GL." << std::endl;
            glfwDestroyWindow(window);
            return nullptr;
        }

        if (!ImGuiLayer::init(window))
        {
            glfwDestroyWindow(window);
            return nullptr;
        }
        // The hub is one window; docking layout persistence belongs to the editor.
        ImGui::GetIO().IniFilename = nullptr;

        ensureDefaultPath();
        ProjectRegistry::load();

        std::shared_ptr<Project> chosen;

        while (chosen == nullptr && !glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            int width = 0, height = 0;
            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);
            glClearColor(0.043f, 0.055f, 0.067f, 1.0f);   // Palette::Void
            glClear(GL_COLOR_BUFFER_BIT);

            ImGuiLayer::begin();

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 20.0f));

            ImGui::Begin("##hub", nullptr,
                         ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoBringToFrontOnFocus);

            ImGui::PushStyleColor(ImGuiCol_Text, P::Accent);
            ImGui::SetWindowFontScale(1.6f);
            ImGui::TextUnformatted("ScrapEngine");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
            ImGui::TextUnformatted("Open a project, or create a new one.");
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            const float columnWidth = (ImGui::GetContentRegionAvail().x - 24.0f) * 0.5f;

            // --- recent projects -------------------------------------------
            ImGui::BeginChild("##recent", ImVec2(columnWidth, -46.0f));
            heading("RECENT PROJECTS");

            const auto& recents = ProjectRegistry::recents();
            if (recents.empty())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
                ImGui::TextWrapped("Nothing yet. Create a project to get started.");
                ImGui::PopStyleColor();
            }

            std::string forgetPath;
            for (const auto& entry : recents)
            {
                ImGui::PushID(entry.path.c_str());

                // A project whose folder has moved stays listed but is not openable -
                // silently dropping it would be more confusing than showing it greyed.
                ImGui::BeginDisabled(!entry.exists);
                if (ImGui::Button(entry.name.c_str(), ImVec2(-1.0f, 0.0f)))
                {
                    if (auto project = Project::open(entry.path))
                    {
                        chosen = project;
                    }
                    else
                    {
                        setStatus("Could not open " + entry.name, true);
                    }
                }
                ImGui::EndDisabled();

                ImGui::PushStyleColor(ImGuiCol_Text, entry.exists ? P::InkFaint : P::Warning);
                ImGui::TextWrapped("%s", entry.exists ? entry.path.c_str()
                                                      : (entry.path + "  (missing)").c_str());
                ImGui::PopStyleColor();

                if (ImGui::BeginPopupContextItem("##recentCtx"))
                {
                    if (ImGui::MenuItem("Remove from list")) forgetPath = entry.path;
                    ImGui::EndPopup();
                }

                ImGui::Spacing();
                ImGui::PopID();
            }
            if (!forgetPath.empty()) ProjectRegistry::forget(forgetPath);

            ImGui::EndChild();

            ImGui::SameLine(0.0f, 24.0f);

            // --- create / open ---------------------------------------------
            ImGui::BeginChild("##actions", ImVec2(columnWidth, -46.0f));

            heading("NEW PROJECT");
            ImGui::TextUnformatted("Name");
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::InputText("##name", newProjectName, sizeof(newProjectName));

            ImGui::TextUnformatted("Location");
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::InputText("##path", newProjectPath, sizeof(newProjectPath));

            ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
            ImGui::TextWrapped("Created at %s/%s", newProjectPath, newProjectName);
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Button, P::AccentDim);
            ImGui::PushStyleColor(ImGuiCol_Text, P::AccentHover);
            if (ImGui::Button("Create Project", ImVec2(-1.0f, 30.0f)))
            {
                if (std::strlen(newProjectName) == 0)
                {
                    setStatus("A project needs a name.", true);
                }
                else
                {
                    const fs::path target = fs::path(newProjectPath) / newProjectName;
                    if (auto project = Project::create(target, newProjectName))
                    {
                        chosen = project;
                    }
                    else
                    {
                        setStatus("Could not create there - the folder exists and is not empty.",
                                  true);
                    }
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            heading("OPEN EXISTING");
            ImGui::TextUnformatted("Project folder or .scrapproject file");
            ImGui::SetNextItemWidth(-1.0f);
            const bool submitted = ImGui::InputText("##open", openPath, sizeof(openPath),
                                                    ImGuiInputTextFlags_EnterReturnsTrue);
            if (ImGui::Button("Open", ImVec2(-1.0f, 0.0f)) || submitted)
            {
                if (std::strlen(openPath) == 0) setStatus("Enter a path to open.", true);
                else if (auto project = Project::open(openPath)) chosen = project;
                else setStatus("No project found at that path.", true);
            }

            ImGui::EndChild();

            // --- status ----------------------------------------------------
            if (!statusMessage.empty())
            {
                ImGui::Separator();
                ImGui::PushStyleColor(ImGuiCol_Text, statusIsError ? P::Danger : P::Success);
                ImGui::TextWrapped("%s", statusMessage.c_str());
                ImGui::PopStyleColor();
            }

            ImGui::End();
            ImGui::PopStyleVar(2);

            ImGuiLayer::end(width, height);
            glfwSwapBuffers(window);
        }

        ImGuiLayer::shutdown();
        glfwDestroyWindow(window);

        if (chosen)
        {
            Project::setActive(chosen);
            ProjectRegistry::remember(chosen);
        }
        return chosen;
    }
}
