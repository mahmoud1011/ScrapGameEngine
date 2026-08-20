#include "Panels.h"
#include "EditorContext.h"
#include "ScrapTheme.h"

#include "renderer/Renderer.h"
#include "renderer/Renderer3D.h"
#include "rhi/Framebuffer.h"
#include "scene/Entity.h"
#include "scene/Scene.h"

#ifdef SCRAP_HAS_DOTNET
#include "scripting/ScriptEngine.h"
#endif

#include <entt/entt.hpp>

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace ScrapGameEngine;
using Scrap::Entity;

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
            if (playing) { ctx.onStop(); ctx.logInfo("Exited play mode - edits restored."); }
            else         { ctx.onPlay(); ctx.logInfo("Entered play mode on a scene copy."); }
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
        if (toolToggle("Cull", ctx.cullingEnabled,
                       "Frustum culling - toggle to compare draw counts"))
        {
            ctx.cullingEnabled = !ctx.cullingEnabled;
            Renderer3D::setCullingEnabled(ctx.cullingEnabled);
        }
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
            auto& target = ctx.sceneTarget;
            target.resize(static_cast<unsigned int>(avail.x), static_cast<unsigned int>(avail.y));
            ctx.camera.setViewportSize(avail.x, avail.y);

            // The colour attachment is just a GL texture, so it drops straight into an
            // ImGui image. UVs are flipped because GL's origin is bottom-left.
            ImGui::Image(static_cast<ImTextureID>(target.getColorAttachment()),
                         avail, ImVec2(0, 1), ImVec2(1, 0));

            // Picking: read the entity id under the cursor. Only on click, because a
            // pixel read stalls the pipeline waiting on the GPU.
            if (ctx.viewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
                !ImGuizmo::IsOver())
            {
                const ImVec2 mouse = ImGui::GetMousePos();
                const int px = static_cast<int>(mouse.x - origin.x);
                // Flip Y: the framebuffer's origin is bottom-left, the panel's is top-left.
                const int py = static_cast<int>(avail.y - (mouse.y - origin.y));

                const int id = target.readEntityId(px, py);
                if (id < 0) ctx.clearSelection();
                else ctx.select(Entity{static_cast<entt::entity>(id), ctx.activeScene.get()});
            }

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

            auto& transform = ctx.selection.getComponent<Scrap::TransformComponent>();
            glm::mat4 matrix = transform.matrix();

            const ImGuizmo::OPERATION op =
                ctx.gizmoOp == GizmoOp::Translate ? ImGuizmo::TRANSLATE :
                ctx.gizmoOp == GizmoOp::Rotate    ? ImGuizmo::ROTATE : ImGuizmo::SCALE;

            const float snapValue = ctx.gizmoOp == GizmoOp::Translate ? ctx.translateSnap :
                                    ctx.gizmoOp == GizmoOp::Rotate    ? ctx.rotateSnap : ctx.scaleSnap;
            const float snap[3] = {snapValue, snapValue, snapValue};

            const glm::mat4 view = ctx.camera.getView();
            const glm::mat4 projection = ctx.camera.getProjection();

            if (ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), op,
                                     ctx.gizmoLocalSpace ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
                                     glm::value_ptr(matrix), nullptr,
                                     ctx.snapEnabled ? snap : nullptr))
            {
                glm::vec3 t{}, r{}, sc{};
                ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(matrix), glm::value_ptr(t), glm::value_ptr(r), glm::value_ptr(sc));

                // ImGuizmo works in degrees; the component stores radians. Rotation is
                // written as a delta so gimbal wrap does not snap the object around.
                const glm::vec3 deltaRotation = glm::radians(r) - transform.rotation;
                transform.translation = t;
                transform.rotation += deltaRotation;
                transform.scale = sc;
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    // --------------------------------------------------------------- game view

    void Panels::drawGameView()
    {
        auto& ctx = EditorContext::get();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Game");

        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 origin = ImGui::GetCursorScreenPos();
        ctx.gameViewportSize = {avail.x, avail.y};

        if (avail.x > 0.0f && avail.y > 0.0f)
        {
            ctx.gameTarget.resize(static_cast<unsigned int>(avail.x),
                                  static_cast<unsigned int>(avail.y));
            ImGui::Image(static_cast<ImTextureID>(ctx.gameTarget.getColorAttachment()),
                         avail, ImVec2(0, 1), ImVec2(1, 0));

            if (!ctx.gameHasCamera)
            {
                // Say why the view is empty rather than leaving a black rectangle.
                const char* message = "No primary camera in the scene";
                const ImVec2 size = ImGui::CalcTextSize(message);
                ImGui::GetWindowDrawList()->AddText(
                    ImVec2(origin.x + (avail.x - size.x) * 0.5f,
                           origin.y + (avail.y - size.y) * 0.5f),
                    ImGui::GetColorU32(P::Warning), message);
            }

            ImGui::GetWindowDrawList()->AddRect(
                origin, ImVec2(origin.x + avail.x, origin.y + avail.y),
                ImGui::GetColorU32(ctx.isPlaying() ? P::Accent : P::Line));
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

        auto scene = ctx.activeScene;
        if (!scene) { ImGui::End(); return; }

        std::string filter = hierarchyFilter;
        std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);

        int visible = 0;
        for (auto handle : scene->raw().view<Scrap::TagComponent>())
        {
            Entity entity{handle, scene.get()};
            std::string name = entity.getName();

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
            if (ctx.selection == entity) flags |= ImGuiTreeNodeFlags_Selected;

            ImGui::PushID(static_cast<int>(static_cast<uint32_t>(entity)));
            ImGui::TreeNodeEx(name.empty() ? "(unnamed)" : name.c_str(), flags);
            if (ImGui::IsItemClicked()) ctx.select(entity);

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete Entity"))
                {
                    if (ctx.selection == entity) ctx.clearSelection();
                    scene->destroyEntity(entity);
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }

        if (visible == 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
            ImGui::TextWrapped(scene->entityCount() == 0
                ? "No entities in the scene. Right-click here to add one."
                : "Nothing matches that filter.");
            ImGui::PopStyleColor();
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
            !ImGui::IsAnyItemHovered())
        {
            ctx.clearSelection();
        }

        if (ImGui::BeginPopupContextWindow("##hierarchyMenu",
                ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Create Empty")) ctx.select(scene->createEntity("Entity"));
            if (ImGui::MenuItem("Create Sprite"))
            {
                Entity e = scene->createEntity("Sprite");
                e.addComponent<Scrap::SpriteRendererComponent>();
                ctx.select(e);
            }
            if (ImGui::MenuItem("Create Camera"))
            {
                Entity e = scene->createEntity("Camera");
                e.addComponent<Scrap::CameraComponent>();
                ctx.select(e);
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("3D Object"))
            {
                struct Primitive { const char* label; Scrap::PrimitiveKind kind; };
                const Primitive primitives[] = {
                    {"Cube",   Scrap::PrimitiveKind::Cube},
                    {"Sphere", Scrap::PrimitiveKind::Sphere},
                    {"Plane",  Scrap::PrimitiveKind::Plane},
                };
                for (const auto& p : primitives)
                {
                    if (ImGui::MenuItem(p.label))
                    {
                        Entity e = scene->createEntity(p.label);
                        e.addComponent<Scrap::MeshRendererComponent>().primitive = p.kind;
                        ctx.select(e);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Light"))
            {
                if (ImGui::MenuItem("Directional"))
                {
                    Entity e = scene->createEntity("Directional Light");
                    auto& light = e.addComponent<Scrap::LightComponent>();
                    light.kind = Scrap::LightKind::Directional;
                    light.intensity = 3.0f;
                    e.getComponent<Scrap::TransformComponent>().rotation = {-0.9f, 0.5f, 0.0f};
                    ctx.select(e);
                }
                if (ImGui::MenuItem("Point"))
                {
                    Entity e = scene->createEntity("Point Light");
                    e.addComponent<Scrap::LightComponent>();
                    e.getComponent<Scrap::TransformComponent>().translation = {0.0f, 2.0f, 0.0f};
                    ctx.select(e);
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
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

        Entity entity = ctx.selection;

        char nameBuffer[128];
        std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", entity.getName().c_str());
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::InputText("##name", nameBuffer, sizeof(nameBuffer)))
            entity.getComponent<Scrap::TagComponent>().tag = nameBuffer;

        ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
        ImGui::Text("UUID %llu", static_cast<unsigned long long>(entity.getUUID()));
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (auto* t = entity.tryGetComponent<Scrap::TransformComponent>())
        {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                vec3Row("Position", t->translation, 0.0f, 78.0f);

                // Degrees in the UI, radians in the component.
                glm::vec3 degrees = glm::degrees(t->rotation);
                if (vec3Row("Rotation", degrees, 0.0f, 78.0f)) t->rotation = glm::radians(degrees);

                vec3Row("Scale", t->scale, 1.0f, 78.0f);
            }
        }

        if (auto* sprite = entity.tryGetComponent<Scrap::SpriteRendererComponent>())
        {
            bool open = ImGui::CollapsingHeader("Sprite Renderer", ImGuiTreeNodeFlags_DefaultOpen);
            if (ImGui::BeginPopupContextItem("##spriteCtx"))
            {
                if (ImGui::MenuItem("Remove Component"))
                    entity.removeComponent<Scrap::SpriteRendererComponent>();
                ImGui::EndPopup();
            }
            if (open)
            {
                ImGui::ColorEdit4("Color", glm::value_ptr(sprite->color));
                ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
                ImGui::TextWrapped("Texture: %s",
                    sprite->texturePath.empty() ? "(none)" : sprite->texturePath.c_str());
                ImGui::PopStyleColor();
            }
        }

        if (auto* camera = entity.tryGetComponent<Scrap::CameraComponent>())
        {
            bool open = ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen);
            if (ImGui::BeginPopupContextItem("##cameraCtx"))
            {
                if (ImGui::MenuItem("Remove Component"))
                    entity.removeComponent<Scrap::CameraComponent>();
                ImGui::EndPopup();
            }
            if (open)
            {
                int projection = camera->projection == Scrap::ProjectionKind::Perspective ? 1 : 0;
                if (ImGui::Combo("Projection", &projection, "Orthographic\0Perspective\0"))
                {
                    camera->projection = projection == 1 ? Scrap::ProjectionKind::Perspective
                                                         : Scrap::ProjectionKind::Orthographic;
                }
                if (camera->projection == Scrap::ProjectionKind::Orthographic)
                    ImGui::DragFloat("Size", &camera->orthoSize, 0.1f, 0.1f, 1000.0f);
                else
                    ImGui::DragFloat("FOV", &camera->fovDegrees, 0.5f, 1.0f, 179.0f);
                ImGui::Checkbox("Primary", &camera->primary);
            }
        }

        if (auto* mesh = entity.tryGetComponent<Scrap::MeshRendererComponent>())
        {
            bool open = ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen);
            if (ImGui::BeginPopupContextItem("##meshCtx"))
            {
                if (ImGui::MenuItem("Remove Component"))
                    entity.removeComponent<Scrap::MeshRendererComponent>();
                ImGui::EndPopup();
            }
            if (open)
            {
                int primitive = static_cast<int>(mesh->primitive);
                if (ImGui::Combo("Mesh", &primitive, "Cube\0Sphere\0Plane\0Custom\0"))
                {
                    mesh->primitive = static_cast<Scrap::PrimitiveKind>(primitive);
                    mesh->mesh.reset();   // rebuilt lazily on the next draw
                }
                ImGui::ColorEdit4("Albedo", glm::value_ptr(mesh->albedo));
                ImGui::SliderFloat("Metallic", &mesh->metallic, 0.0f, 1.0f);
                ImGui::SliderFloat("Roughness", &mesh->roughness, 0.02f, 1.0f);
                ImGui::SliderFloat("Emissive", &mesh->emissive, 0.0f, 4.0f);
            }
        }

        if (auto* light = entity.tryGetComponent<Scrap::LightComponent>())
        {
            bool open = ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen);
            if (ImGui::BeginPopupContextItem("##lightCtx"))
            {
                if (ImGui::MenuItem("Remove Component"))
                    entity.removeComponent<Scrap::LightComponent>();
                ImGui::EndPopup();
            }
            if (open)
            {
                int kind = light->kind == Scrap::LightKind::Directional ? 0 : 1;
                if (ImGui::Combo("Type", &kind, "Directional\0Point\0"))
                {
                    light->kind = kind == 0 ? Scrap::LightKind::Directional
                                            : Scrap::LightKind::Point;
                }
                ImGui::ColorEdit3("Color", glm::value_ptr(light->color));
                ImGui::DragFloat("Intensity", &light->intensity, 0.05f, 0.0f, 50.0f);
                if (light->kind == Scrap::LightKind::Point)
                    ImGui::DragFloat("Range", &light->range, 0.1f, 0.1f, 200.0f);
                else
                    ImGui::TextColored(P::InkFaint, "Aim by rotating the entity.");
            }
        }

        if (auto* script = entity.tryGetComponent<Scrap::ScriptComponent>())
        {
            bool open = ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen);
            if (ImGui::BeginPopupContextItem("##scriptCtx"))
            {
                if (ImGui::MenuItem("Remove Component"))
                    entity.removeComponent<Scrap::ScriptComponent>();
                ImGui::EndPopup();
            }
            if (open)
            {
                char typeBuffer[192];
                std::snprintf(typeBuffer, sizeof(typeBuffer), "%s", script->typeName.c_str());
                if (ImGui::InputText("Type", typeBuffer, sizeof(typeBuffer)))
                    script->typeName = typeBuffer;
                ImGui::PushStyleColor(ImGuiCol_Text, P::InkFaint);
                ImGui::TextWrapped("Managed type, e.g. Game.Player, GameScripts. "
                                   "Bound when play mode starts.");
                ImGui::PopStyleColor();
            }
        }

        ImGui::Spacing();
        if (ImGui::Button("Add Component", ImVec2(-1.0f, 0.0f))) ImGui::OpenPopup("##addComponent");
        if (ImGui::BeginPopup("##addComponent"))
        {
            if (!entity.hasComponent<Scrap::SpriteRendererComponent>() &&
                ImGui::MenuItem("Sprite Renderer"))
                entity.addComponent<Scrap::SpriteRendererComponent>();
            if (!entity.hasComponent<Scrap::CameraComponent>() && ImGui::MenuItem("Camera"))
                entity.addComponent<Scrap::CameraComponent>();
            if (!entity.hasComponent<Scrap::MeshRendererComponent>() &&
                ImGui::MenuItem("Mesh Renderer"))
                entity.addComponent<Scrap::MeshRendererComponent>();
            if (!entity.hasComponent<Scrap::LightComponent>() && ImGui::MenuItem("Light"))
                entity.addComponent<Scrap::LightComponent>();
            if (!entity.hasComponent<Scrap::ScriptComponent>() && ImGui::MenuItem("Script"))
                entity.addComponent<Scrap::ScriptComponent>();
            ImGui::EndPopup();
        }

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

#ifdef SCRAP_HAS_DOTNET
        ImGui::Spacing();
        sectionLabel("SCRIPTS");
        ImGui::Text("Instances   %d", Scrap::ScriptEngine::liveInstanceCount());
#endif

        ImGui::Spacing();
        sectionLabel("RENDERER 3D");
        {
            const auto& stats3d = Renderer3D::getStats();
            ImGui::Text("Draw calls  %u", stats3d.drawCalls);
            ImGui::Text("Meshes      %u", stats3d.meshCount);
            ImGui::Text("Triangles   %u", stats3d.triangleCount);
            ImGui::Text("Culled      %u", stats3d.culled);
            ImGui::Text("Uniforms    %u", stats3d.uniformUploads);

            const unsigned int considered = stats3d.meshCount + stats3d.culled;
            ImGui::PushStyleColor(ImGuiCol_Text, P::InkMuted);
            if (considered > 0)
                ImGui::Text("%.0f%% culled",
                            100.0f * static_cast<float>(stats3d.culled) /
                            static_cast<float>(considered));
            ImGui::PopStyleColor();
        }

        ImGui::Spacing();
        sectionLabel("SCENE");
        if (ctx.activeScene)
        {
            ImGui::Text("Entities    %zu", ctx.activeScene->entityCount());
            ImGui::Text("Scene       %s", ctx.activeScene->getName().c_str());
        }

        ImGui::End();
    }
}
