#include "ImGuiLayer.h"
#include "ScrapTheme.h"

#include <imgui.h>
#include <imgui_internal.h>   // DockBuilder, for the first-run layout
// vcpkg installs the backends flat rather than under backends/, unlike an upstream
// checkout.
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <iostream>

namespace Scrap::Editor
{
    namespace
    {
        bool dockspaceOpen = true;
    }

    bool ImGuiLayer::init(GLFWwindow* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Panel layout persists between sessions, next to the executable.
        io.IniFilename = "scrap-editor-layout.ini";

        applyScrapTheme();

        // Platform windows are real OS windows, so they must not be translucent or
        // rounded differently from the main one.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
        {
            std::cerr << "[EDITOR] ImGui GLFW backend failed to initialise." << std::endl;
            return false;
        }

        // Matches the engine's 3.3 core context.
        if (!ImGui_ImplOpenGL3_Init("#version 330 core"))
        {
            std::cerr << "[EDITOR] ImGui OpenGL3 backend failed to initialise." << std::endl;
            ImGui_ImplGlfw_Shutdown();
            return false;
        }

        return true;
    }

    void ImGuiLayer::shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::end(int displayWidth, int displayHeight)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(displayWidth), static_cast<float>(displayHeight));

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }
    }

    void ImGuiLayer::beginDockspace()
    {
        static constexpr ImGuiWindowFlags hostFlags =
            ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("##ScrapDockHost", &dockspaceOpen, hostFlags);
        ImGui::PopStyleVar(3);

        const ImGuiID dockspaceId = ImGui::GetID("ScrapDockspace");

        // First run has no saved layout, so build the Unity-shaped arrangement:
        // toolbar strip on top, hierarchy left, inspector right, content and console
        // below, viewport in the centre. After this the .ini takes over and the user's
        // own layout is what persists.
        if (ImGui::DockBuilderGetNode(dockspaceId) == nullptr)
        {
            ImGui::DockBuilderRemoveNode(dockspaceId);
            ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->WorkSize);

            ImGuiID centre = dockspaceId;
            const ImGuiID toolbar = ImGui::DockBuilderSplitNode(centre, ImGuiDir_Up, 0.045f, nullptr, &centre);
            const ImGuiID left    = ImGui::DockBuilderSplitNode(centre, ImGuiDir_Left, 0.190f, nullptr, &centre);
            const ImGuiID right   = ImGui::DockBuilderSplitNode(centre, ImGuiDir_Right, 0.260f, nullptr, &centre);
            const ImGuiID bottom  = ImGui::DockBuilderSplitNode(centre, ImGuiDir_Down, 0.300f, nullptr, &centre);

            ImGui::DockBuilderDockWindow("##Toolbar", toolbar);
            ImGui::DockBuilderDockWindow("Hierarchy", left);
            ImGui::DockBuilderDockWindow("Inspector", right);
            ImGui::DockBuilderDockWindow("Stats", right);
            ImGui::DockBuilderDockWindow("Content", bottom);
            ImGui::DockBuilderDockWindow("Console", bottom);
            ImGui::DockBuilderDockWindow("Viewport", centre);

            // The toolbar is a fixed strip, not something to tab or resize away.
            if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(toolbar))
            {
                node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar |
                                    ImGuiDockNodeFlags_NoDockingOverMe |
                                    ImGuiDockNodeFlags_NoResizeY;
            }

            ImGui::DockBuilderFinish(dockspaceId);
        }

        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void ImGuiLayer::endDockspace()
    {
        ImGui::End();
    }

    bool ImGuiLayer::wantsKeyboard() { return ImGui::GetIO().WantCaptureKeyboard; }
    bool ImGuiLayer::wantsMouse()    { return ImGui::GetIO().WantCaptureMouse; }
}
