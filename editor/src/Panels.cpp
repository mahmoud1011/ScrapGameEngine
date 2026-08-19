#include "Panels.h"
#include "EditorContext.h"
#include "ScrapTheme.h"

#include "renderer/Renderer.h"
#include "rhi/Framebuffer.h"
#include "scene/GameObject.h"
#include "scene/GameObjectCollection.h"
#include "scene/Transform.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace ScrapGameEngine;

namespace Scrap::Editor
{
    namespace
    {
        std::filesystem::path contentRoot = ".";
        std::filesystem::path contentCurrent = ".";
        char hierarchyFilter[64] = "";

        using P = Palette;

        /** Uppercase, letter-spaced section label. */
        void sectionLabel(const char* text)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, P::InkMuted);
            ImGui::SetWindowFontScale(0.85f);
            ImGui::TextUnformatted(text);
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();
        }

        /** A toggle that reads as pressed when active, using the accent. */
        bool toolToggle(const char* label, bool active, const char* tooltip = nullptr)
        {
            if (active)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, P::AccentDim);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, P::AccentDim);
                ImGui::PushStyleColor(ImGuiCol_Text, P::AccentHover);
            }
            const bool pressed = ImGui::Button(label);
            if (active) ImGui::PopStyleColor(3);

            if (tooltip && ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", tooltip);
            return pressed;
        }

        /**
         * A labelled XYZ row where each axis button carries its gizmo colour, so an
         * axis reads the same in the inspector as it does on the handle.
         */
        bool vec3Row(const char* label, glm::vec3& values, float resetTo, float columnWidth)
        {
            bool changed = false;
            ImGui::PushID(label);

            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(label);
            ImGui::NextColumn();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3.0f, 0.0f));

            const float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
            const ImVec2 buttonSize = {lineHeight + 4.0f, lineHeight};

            // Widths are computed here rather than with PushMultiItemsWidths, which
            // lives in imgui_internal.h - not worth taking a dependency on internals
            // for three drag fields.
            const float spacing = 3.0f;
            const float dragWidth = std::max(
                24.0f,
                (ImGui::GetContentRegionAvail().x - 3.0f * (buttonSize.x + spacing * 2.0f)) / 3.0f);

            struct Axis { const char* name; ImVec4 color; float* value; };
            const Axis axes[3] = {
                {"X", P::AxisX, &values.x},
                {"Y", P::AxisY, &values.y},
                {"Z", P::AxisZ, &values.z},
            };

            for (int i = 0; i < 3; i++)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, axes[i].color);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, axes[i].color);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, axes[i].color);
                ImGui::PushStyleColor(ImGuiCol_Text, P::Void);
                if (ImGui::Button(axes[i].name, buttonSize))
                {
                    *axes[i].value = resetTo;
                    changed = true;
                }
                ImGui::PopStyleColor(4);

                ImGui::SameLine();
                ImGui::PushID(i);
                ImGui::SetNextItemWidth(dragWidth);
                if (ImGui::DragFloat("##v", axes[i].value, 0.05f, 0.0f, 0.0f, "%.3f"))
                    changed = true;
                ImGui::PopID();
                if (i < 2) ImGui::SameLine();
            }

            ImGui::PopStyleVar();
            ImGui::Columns(1);
            ImGui::PopID();
            return changed;
        }
    }

    void Panels::setContentRoot(const std::filesystem::path& root)
    {
        contentRoot = root;
        contentCurrent = root;
    }

    // ------------------------------------------------------------------ toolbar

    void Panels::drawToolbar()
    {
        auto& ctx = EditorContext::get();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 5.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, P::Sunken);
        ImGui::Begin("##Toolbar", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse);

        // Play / pause / stop. Play turns accent while running, so the editor's state
        // is legible from across the room.
        const bool playing = ctx.playState == PlayState::Playing;
        if (toolToggle(playing ? "Stop" : "Play", playing, playing ? "Stop playing" : "Enter play mode"))
        {
            ctx.playState = playing ? PlayState::Edit : PlayState::Playing;
            ctx.logInfo(playing ? "Exited play mode." : "Entered play mode.");
        }
        ImGui::SameLine();

        ImGui::BeginDisabled(ctx.playState == PlayState::Edit);
        if (toolToggle("Pause", ctx.playState == PlayState::Paused, "Pause the running scene"))
        {
            ctx.playState = (ctx.playState == PlayState::Paused) ? PlayState::Playing : PlayState::Paused;
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::TextColored(P::Line, "|");
        ImGui::SameLine();

        // Gizmo mode.
        if (toolToggle("Move", ctx.gizmoOp == GizmoOp::Translate, "Translate (W)"))
            ctx.gizmoOp = GizmoOp::Translate;
        ImGui::SameLine();
        if (toolToggle("Rotate", ctx.gizmoOp == GizmoOp::Rotate, "Rotate (E)"))
            ctx.gizmoOp = GizmoOp::Rotate;
        ImGui::SameLine();
        if (toolToggle("Scale", ctx.gizmoOp == GizmoOp::Scale, "Scale (R)"))
            ctx.gizmoOp = GizmoOp::Scale;

        ImGui::SameLine();
        ImGui::TextColored(P::Line, "|");
        ImGui::SameLine();

        if (toolToggle(ctx.gizmoLocalSpace ? "Local" : "World", ctx.gizmoLocalSpace,
                       "Toggle gizmo space"))
            ctx.gizmoLocalSpace = !ctx.gizmoLocalSpace;
        ImGui::SameLine();
        if (toolToggle("Snap", ctx.snapEnabled, "Snap to increments while dragging"))
            ctx.snapEnabled = !ctx.snapEnabled;
        ImGui::SameLine();
        if (toolToggle("Grid", ctx.showGrid, "Show the viewport grid"))
            ctx.showGrid = !ctx.showGrid;
        ImGui::SameLine();
        if (toolToggle(ctx.camera.isPerspective() ? "3D" : "2D", ctx.camera.isPerspective(),
                       "Toggle the editor camera projection"))
            ctx.camera.setPerspective(!ctx.camera.isPerspective());

        // Frame timing, right-aligned.
        char timing[64];
        std::snprintf(timing, sizeof(timing), "%.1f fps   %.2f ms", ctx.fps, ctx.frameMs);
        const float textWidth = ImGui::CalcTextSize(timing).x;
        ImGui::SameLine(ImGui::GetWindowWidth() - textWidth - 14.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, P::InkMuted);
        ImGui::TextUnformatted(timing);
        ImGui::PopStyleColor();

        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    // ----------------------------------------------------------------- viewport

    void Panels::drawViewport()
    {
        auto& ctx = EditorContext::get();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        ctx.viewportFocused = ImGui::IsWindowFocused();
        ctx.viewportHovered = ImGui::IsWindowHovered();

        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 origin = ImGui::GetCursorScreenPos();
        ctx.viewportSize = {avail.x, avail.y};
        ctx.viewportBoundsMin = {origin.x, origin.y};

        if (avail.x > 0.0f && avail.y > 0.0f)
        {
            auto& target = Renderer::getSceneTarget();
            target.resize(static_cast<unsigned int>(avail.x), static_cast<unsigned int>(avail.y));
            ctx.camera.setViewportSize(avail.x, avail.y);

            // The colour attachment is just a GL texture, so it drops straight into an
            // ImGui image. UVs are flipped because GL's origin is bottom-left.
            ImGui::Image(static_cast<ImTextureID>(target.getColorAttachment()),
                         avail, ImVec2(0, 1), ImVec2(1, 0));

            // A hairline frame keeps the viewport visually distinct from the chrome.
            ImGui::GetWindowDrawList()->AddRect(
                origin, ImVec2(origin.x + avail.x, origin.y + avail.y),
                ImGui::GetColorU32(ctx.viewportFocused ? P::Accent : P::Line));
        }

        // ImGuizmo draws in screen space over the image.
        if (ctx.hasSelection() && ctx.gizmoOp != GizmoOp::None && avail.x > 0.0f)
        {
            ImGuizmo::SetOrthographic(!ctx.camera.isPerspective());
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(origin.x, origin.y, avail.x, avail.y);

            Transform* transform = ctx.selection->transform;
            const glm::vec2 pos = transform->getPosition();
            const glm::vec2 scale = transform->getLocalScale();
            const float rotation = transform->getLocalRotation();

            // The engine's Transform is still 2D, so the matrix is assembled here and
            // decomposed back afterwards. D3's vec3 TransformComponent removes this.
            glm::mat4 matrix(1.0f);
            ImGuizmo::RecomposeMatrixFromComponents(
                glm::value_ptr(glm::vec3(pos.x, pos.y, 0.0f)),
                glm::value_ptr(glm::vec3(0.0f, 0.0f, rotation)),
                glm::value_ptr(glm::vec3(scale.x, scale.y, 1.0f)),
                glm::value_ptr(matrix));

            const ImGuizmo::OPERATION op =
                ctx.gizmoOp == GizmoOp::Translate ? ImGuizmo::TRANSLATE :
                ctx.gizmoOp == GizmoOp::Rotate    ? ImGuizmo::ROTATE : ImGuizmo::SCALE;

            float snapValue = ctx.gizmoOp == GizmoOp::Translate ? ctx.translateSnap :
                              ctx.gizmoOp == GizmoOp::Rotate    ? ctx.rotateSnap : ctx.scaleSnap;
            const float snap[3] = {snapValue, snapValue, snapValue};

            const glm::mat4 view = ctx.camera.getView();
            const glm::mat4 projection = ctx.camera.getProjection();

            if (ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), op,
                                     ctx.gizmoLocalSpace ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
                                     glm::value_ptr(matrix), nullptr,
                                     ctx.snapEnabled ? snap : nullptr))
            {
                glm::vec3 outT{}, outR{}, outS{};
                ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(matrix), glm::value_ptr(outT),
                    glm::value_ptr(outR), glm::value_ptr(outS));

                transform->setPosition({outT.x, outT.y});
                transform->setRotation(outR.z);
                transform->setScale({outS.x, outS.y});
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    // ---------------------------------------------------------------- hierarchy

    void Panels::drawHierarchy()
    {
        auto& ctx = EditorContext::get();
        ImGui::Begin("Hierarchy");

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##filter", "Filter objects", hierarchyFilter, sizeof(hierarchyFilter));
        ImGui::Spacing();

        const auto& objects = GameObjectCollection::all();
        std::string filter = hierarchyFilter;
        std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);

        int visible = 0;
        for (GameObject* go : objects)
        {
            if (go == nullptr) continue;

            std::string name = go->getName();
            if (!filter.empty())
            {
                std::string lower = name;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                if (lower.find(filter) == std::string::npos) continue;
            }
            visible++;

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
                                       ImGuiTreeNodeFlags_Leaf |
                                       ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if (ctx.selection == go) flags |= ImGuiTreeNodeFlags_Selected;

            ImGui::PushID(go);
            ImGui::TreeNodeEx(name.empty() ? "(unnamed)" : name.c_str(), flags);
            if (ImGui::IsItemClicked()) ctx.select(go);
            ImGui::PopID();
        }

        if (visible == 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
            ImGui::TextWrapped(objects.empty()
                ? "No objects in the scene."
                : "Nothing matches that filter.");
            ImGui::PopStyleColor();
        }

        // Clicking empty space clears the selection, as in most editors.
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
            !ImGui::IsAnyItemHovered())
        {
            ctx.clearSelection();
        }

        ImGui::End();
    }

    // ---------------------------------------------------------------- inspector

    void Panels::drawInspector()
    {
        auto& ctx = EditorContext::get();
        ImGui::Begin("Inspector");

        if (!ctx.hasSelection())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
            ImGui::TextWrapped("Select an object in the Hierarchy to edit it.");
            ImGui::PopStyleColor();
            ImGui::End();
            return;
        }

        GameObject* go = ctx.selection;

        char nameBuffer[128];
        std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", go->getName().c_str());
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::InputText("##name", nameBuffer, sizeof(nameBuffer)))
            go->setName(nameBuffer);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Transform* t = go->transform;

            glm::vec2 pos2 = t->getPosition();
            glm::vec3 pos{pos2.x, pos2.y, 0.0f};
            if (vec3Row("Position", pos, 0.0f, 78.0f))
                t->setPosition({pos.x, pos.y});

            glm::vec3 rot{0.0f, 0.0f, t->getLocalRotation()};
            if (vec3Row("Rotation", rot, 0.0f, 78.0f))
                t->setRotation(rot.z);

            glm::vec2 scale2 = t->getLocalScale();
            glm::vec3 scale{scale2.x, scale2.y, 1.0f};
            if (vec3Row("Scale", scale, 1.0f, 78.0f))
                t->setScale({scale.x, scale.y});

            // Z is inert until D3 lands the vec3 TransformComponent; saying so beats
            // letting someone drag it and wonder why nothing moves.
            ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
            ImGui::TextWrapped("Z is inactive - Transform is still 2D until the unified "
                               "3D transform lands.");
            ImGui::PopStyleColor();
        }

        ImGui::Spacing();
        sectionLabel("COMPONENTS");
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
        ImGui::TextWrapped("Per-component inspectors arrive with the ECS: reflection over "
                           "a component registry replaces hand-written panels.");
        ImGui::PopStyleColor();

        ImGui::End();
    }

    // ---------------------------------------------------------- content browser

    void Panels::drawContentBrowser()
    {
        ImGui::Begin("Content");

        namespace fs = std::filesystem;

        if (contentCurrent != contentRoot)
        {
            if (ImGui::Button("< Back")) contentCurrent = contentCurrent.parent_path();
            ImGui::SameLine();
        }
        ImGui::PushStyleColor(ImGuiCol_Text, P::InkMuted);
        ImGui::TextUnformatted(contentCurrent.string().c_str());
        ImGui::PopStyleColor();
        ImGui::Separator();

        std::error_code ec;
        if (!fs::exists(contentCurrent, ec))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, P::Warning);
            ImGui::TextWrapped("Content root not found: %s", contentCurrent.string().c_str());
            ImGui::PopStyleColor();
            ImGui::End();
            return;
        }

        // Grid of tiles, directories first.
        const float tile = 84.0f;
        const float cell = tile + ImGui::GetStyle().ItemSpacing.x;
        const int columns = std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / cell));

        ImGui::Columns(columns, nullptr, false);

        std::vector<fs::directory_entry> entries;
        for (const auto& e : fs::directory_iterator(contentCurrent, ec)) entries.push_back(e);
        std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
            if (a.is_directory() != b.is_directory()) return a.is_directory();
            return a.path().filename() < b.path().filename();
        });

        for (const auto& entry : entries)
        {
            const bool isDir = entry.is_directory();
            const std::string filename = entry.path().filename().string();

            ImGui::PushID(filename.c_str());
            ImGui::PushStyleColor(ImGuiCol_Button, P::Surface);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, P::Raised);
            ImGui::PushStyleColor(ImGuiCol_Text, isDir ? P::Accent : P::InkMuted);

            if (ImGui::Button(isDir ? "[DIR]" : "[FILE]", ImVec2(tile, tile)) && isDir)
                contentCurrent = entry.path();

            ImGui::PopStyleColor(3);
            ImGui::TextWrapped("%s", filename.c_str());
            ImGui::PopID();
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::End();
    }

    // ------------------------------------------------------------------ console

    void Panels::drawConsole()
    {
        auto& ctx = EditorContext::get();
        ImGui::Begin("Console");

        if (ImGui::Button("Clear")) ctx.clearLog();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, P::InkMuted);
        ImGui::Text("%zu messages", ctx.logEntries().size());
        ImGui::PopStyleColor();
        ImGui::Separator();

        ImGui::BeginChild("##log", ImVec2(0, 0), ImGuiChildFlags_None,
                          ImGuiWindowFlags_HorizontalScrollbar);
        for (const auto& entry : ctx.logEntries())
        {
            ImVec4 color = P::Ink;
            const char* tag = "INFO ";
            if (entry.level == LogEntry::Level::Warning) { color = P::Warning; tag = "WARN "; }
            else if (entry.level == LogEntry::Level::Error) { color = P::Danger; tag = "ERROR"; }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(tag);
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::TextWrapped("%s", entry.message.c_str());
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1.0f)
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        ImGui::End();
    }

    // -------------------------------------------------------------------- stats

    void Panels::drawStats()
    {
        auto& ctx = EditorContext::get();
        ImGui::Begin("Stats");

        sectionLabel("FRAME");
        ImGui::Text("%.2f ms", ctx.frameMs);
        ImGui::Text("%.1f fps", ctx.fps);

        ImGui::Spacing();
        sectionLabel("RENDERER 2D");
        ImGui::Text("Draw calls  %u", Renderer::getDrawCallCount());
        ImGui::Text("Quads       %u", Renderer::getQuadCount());

        const unsigned int quads = Renderer::getQuadCount();
        const unsigned int calls = Renderer::getDrawCallCount();
        ImGui::PushStyleColor(ImGuiCol_Text, P::InkMuted);
        if (calls > 0)
            ImGui::Text("%.0f quads per call", static_cast<float>(quads) / static_cast<float>(calls));
        else
            ImGui::TextUnformatted("nothing submitted");
        ImGui::PopStyleColor();

        ImGui::Spacing();
        sectionLabel("SCENE");
        ImGui::Text("Objects     %zu", GameObjectCollection::all().size());

        ImGui::End();
    }
}
