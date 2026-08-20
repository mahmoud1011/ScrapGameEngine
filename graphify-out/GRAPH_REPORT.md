# Graph Report - scrap-engine-system-design-c7c2af  (2026-08-20)

## Corpus Check
- 110 files · ~212,714 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 1357 nodes · 2202 edges · 73 communities (69 shown, 4 thin omitted)
- Extraction: 96% EXTRACTED · 4% INFERRED · 0% AMBIGUOUS · INFERRED: 96 edges (avg confidence: 0.81)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- Render Frame and Targets
- C# Scripting API
- Batched Quad Renderer
- Audio Device and Sources
- Vertex and Index Buffers
- Camera Projection
- Application and Window
- Input Handling
- Editor Camera
- Texture Resources
- Shader Compilation
- Text and Glyphs
- Mesh Allocation
- Transform Hierarchy
- Editor Context and Play State
- Tweening and Easing
- Community 16
- Community 17
- Community 18
- Community 19
- Community 20
- Community 21
- Community 22
- Community 23
- Community 24
- Community 25
- Community 26
- Community 27
- Community 28
- Community 29
- Community 30
- Community 31
- Community 32
- Community 33
- Community 34
- Community 35
- Community 36
- Community 37
- Community 38
- Community 39
- Community 40
- Community 41
- Community 42
- Community 43
- Community 44
- Community 45
- Community 46
- Community 47
- Community 48
- Community 49
- Community 50
- Community 51
- Community 52
- Community 53
- Community 54
- Community 55
- Community 56
- Community 57
- Community 58
- Community 59
- Community 60
- Community 61
- Community 62
- Community 63
- Community 64
- Community 65
- Community 67
- Community 68
- Community 69
- Community 70
- Community 71

## God Nodes (most connected - your core abstractions)
1. `EditorContext` - 51 edges
2. `EditorCamera` - 34 edges
3. `Camera` - 31 edges
4. `Text` - 27 edges
5. `BaseComponent` - 26 edges
6. `Renderer2DData` - 25 edges
7. `Renderer3DData` - 24 edges
8. `Shader` - 24 edges
9. `BaseScene` - 24 edges
10. `TweenComponent` - 23 edges

## Surprising Connections (you probably didn't know these)
- `main()` --calls--> `deserialize`  [INFERRED]
  editor/src/main.cpp → engine/src/scene/SceneSerializer.h
- `main()` --calls--> `serialize`  [INFERRED]
  editor/src/main.cpp → engine/src/scene/SceneSerializer.h
- `main()` --calls--> `run`  [INFERRED]
  sandbox/src/main.cpp → engine/src/core/Application.h
- `onInitialize` --calls--> `startTween`  [INFERRED]
  sandbox/src/SplashScreenScene.h → engine/src/scene/TweenComponent.h
- `CI Cross-Platform Build Matrix` --conceptually_related_to--> `OpenGL 3.3 Core Baseline`  [INFERRED]
  .github/workflows/ci.yml → docs/ENGINE_IMPROVEMENT_PLAN.md

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **What Makes the Editor Able to Save** — engine_src_scene_scene_cpp_engine_src_scene_scene, engine_src_scene_sceneserializer_cpp_engine_src_scene_sceneserializer, docs_engine_improvement_plan_uuid_not_entt_handle, docs_engine_improvement_plan_yaml_scene_format, docs_engine_improvement_plan_b7_no_serialization [EXTRACTED 1.00]
- **What Made the Editor Possible** — editor_src_imguilayer_cpp_editor_src_imguilayer, editor_src_panels_cpp_editor_src_panels, docs_engine_improvement_plan_viewport_samples_fbo, docs_engine_improvement_plan_offscreen_scene_target, editor_src_editorcamera_cpp_editor_src_editorcamera [EXTRACTED 1.00]
- **Managed Scripting Stack** — engine_src_scripting_dotnethost_cpp_engine_src_scripting_dotnethost, docs_engine_improvement_plan_d10_coreclr_scripting, docs_engine_improvement_plan_coreclr_over_nativeaot, docs_engine_improvement_plan_blittable_interop_boundary, docs_engine_improvement_plan_optional_dotnet_dependency [EXTRACTED 1.00]
- **Two Renderers, One Frame** — engine_src_renderer_renderer3d_cpp_engine_src_renderer_renderer3d, engine_src_scene_scene_cpp_engine_src_scene_scene, docs_engine_improvement_plan_depth_first_then_sprites, docs_engine_improvement_plan_batched_quad_renderer, docs_engine_improvement_plan_offscreen_scene_target [EXTRACTED 1.00]
- **Why It Could Not Build** — docs_engine_improvement_plan_b2_absolute_include_paths, docs_engine_improvement_plan_b3_missing_link_libraries, docs_engine_improvement_plan_b13_msbuild_only, docs_engine_improvement_plan_d7_cmake_vcpkg [EXTRACTED 1.00]
- **Phase 1 Shader Pipeline** — engine_src_rhi_shader_cpp_engine_src_rhi_shader, engine_src_rhi_buffer_cpp_engine_src_rhi_buffer, engine_src_rhi_vertexarray_cpp_engine_src_rhi_vertexarray, engine_src_renderer_renderer2d_cpp_engine_src_renderer_renderer2d, docs_engine_improvement_plan_batched_quad_renderer [EXTRACTED 1.00]
- **How a Scene Survives a Reload** — engine_src_scene_sceneserializer_cpp_engine_src_scene_sceneserializer, docs_engine_improvement_plan_uuid_over_entt_handles, docs_engine_improvement_plan_d5_yaml_serialization, docs_engine_improvement_plan_copy_on_play [EXTRACTED 1.00]
- **The Closed Scripting Loop** — engine_src_scripting_scriptengine_cpp_engine_src_scripting_scriptengine, scripting_scrapscript_bootstrap_scrap_bootstrap, docs_engine_improvement_plan_uuid_resolved_callbacks, docs_engine_improvement_plan_script_failure_isolation, docs_engine_improvement_plan_copy_on_play [EXTRACTED 1.00]
- **Claims Backed by Headless Tests** — docs_engine_improvement_plan_headless_selftests, engine_src_scene_sceneserializer_cpp_engine_src_scene_sceneserializer, engine_src_scripting_scriptengine_cpp_engine_src_scripting_scriptengine, docs_engine_improvement_plan_copy_on_play [EXTRACTED 1.00]
- **Cross-Platform Constraint Set** — readme_platform_targets, docs_engine_improvement_plan_d2a_opengl_33_core, docs_engine_improvement_plan_d8_miniaudio, _github_workflows_ci_matrix_build, docs_engine_improvement_plan_d7_cmake_vcpkg [INFERRED 0.85]
- **Editor Prerequisite Chain** — docs_engine_improvement_plan_b1_fixed_function_gl, docs_engine_improvement_plan_d2_rhi_two_renderers, docs_engine_improvement_plan_d2a_opengl_33_core, docs_engine_improvement_plan_d6_imgui_editor, docs_engine_improvement_plan_b4_static_god_classes [INFERRED 0.85]
- **What Makes an Editor Viewport Possible** — engine_src_rhi_framebuffer_cpp_engine_src_rhi_framebuffer, docs_engine_improvement_plan_offscreen_scene_target, docs_engine_improvement_plan_d6_imgui_editor, docs_engine_improvement_plan_d4_camera_as_component, docs_engine_improvement_plan_b4_static_god_classes [INFERRED 0.85]
- **Latent Bugs Found Only by Using the Code** — docs_engine_improvement_plan_uninitialised_sprite_pointers, docs_engine_improvement_plan_collection_was_dead_code, docs_engine_improvement_plan_b11_member_init_order, docs_engine_improvement_plan_time_h_shadows_ctime [INFERRED 0.85]

## Communities (73 total, 4 thin omitted)

### Community 0 - "Render Frame and Targets"
Cohesion: 0.06
Nodes (49): Panels::drawStats(), sectionLabel(), run, vec3, RenderParams, rotationZ, scale, texture (+41 more)

### Community 1 - "C# Scripting API"
Cohesion: 0.07
Nodes (20): bool, Scrap, Game, Dictionary, MethodImpl, Entity, Log, ScriptableEntity (+12 more)

### Community 2 - "Batched Quad Renderer"
Cohesion: 0.07
Nodes (44): Switch-Indexed Sampler Array, White Texture in Slot 0, array, mat4, Texture2D, vec2, vec3, vec4 (+36 more)

### Community 3 - "Audio Device and Sources"
Cohesion: 0.07
Nodes (41): scrap_warnings Interface Target, ScrapEngine CMake Project, Shared AudioDevice, B13: MSBuild Only, No CI, B2: Absolute Include Paths, B3: Missing Link Libraries, B5: Game Compiled Into Engine, D7: CMake and vcpkg (+33 more)

### Community 4 - "Vertex and Index Buffers"
Cohesion: 0.06
Nodes (33): const_iterator, Layout-Driven Vertex Attributes, BufferElement, name, normalized, offset, size, type (+25 more)

### Community 5 - "Camera Projection"
Cohesion: 0.08
Nodes (43): Camera, aspectRatio, config, getAspectRatio, getMatrix_projection, getMatrix_view, getMatrix_viewProjection, getOrthoSize (+35 more)

### Community 6 - "Application and Window"
Cohesion: 0.06
Nodes (37): AppWindowEventType, B10: Busy-Wait Frame Limiter, B11: Member Initialisation Order Bug, shutdown, Application, cleanup, frameTime, init (+29 more)

### Community 7 - "Input Handling"
Cohesion: 0.07
Nodes (38): KeyCode, MouseButtonCode, vec2, KeyCode, MouseButtonCode, Input, getKey, getKeyDown (+30 more)

### Community 8 - "Editor Camera"
Cohesion: 0.07
Nodes (29): B4: Static God Classes, D4: Camera as Component, Play Mode Renders Through the Scene Camera, vec2, EditorCamera, dirty, farClip, focalPoint (+21 more)

### Community 9 - "Texture Resources"
Cohesion: 0.10
Nodes (35): string, Texture2D, TextureFilterMode, TextureWrapMode, string, TextureFilterMode, TextureWrapMode, vec4 (+27 more)

### Community 10 - "Shader Compilation"
Cohesion: 0.12
Nodes (29): CI Cross-Platform Build Matrix, B1: Fixed-Function OpenGL, D2: One RHI, Two Renderers, OpenGL 3.3 Core Baseline, glad Regenerated for 3.3 Core, RHI Abstraction Layer, compileStage(), mat4 (+21 more)

### Community 11 - "Text and Glyphs"
Cohesion: 0.08
Nodes (32): GameObject, string, Glyph, advance, offset, size, textureCoords, string (+24 more)

### Community 12 - "Mesh Allocation"
Cohesion: 0.08
Nodes (25): vector, unique_ptr, vec2, vec3, Mesh, getID, getVertexCount, id (+17 more)

### Community 13 - "Transform Hierarchy"
Cohesion: 0.09
Nodes (30): GameObject, vec2, vec2, vector, ScrapGameEngine::Transform::calculateWorldScale(), ScrapGameEngine::Transform::getLocalScale(), ScrapGameEngine::Transform::getParent(), ScrapGameEngine::Transform::getPosition() (+22 more)

### Community 14 - "Editor Context and Play State"
Cohesion: 0.07
Nodes (26): EditorContext, activeScene, camera, editorScene, entries, fps, frameMs, gizmoLocalSpace (+18 more)

### Community 15 - "Tweening and Easing"
Cohesion: 0.09
Nodes (29): EasingType, function, GameObject, TweenType, vec3, getEasingFunction(), EasingType, function (+21 more)

### Community 16 - "Community 16"
Cohesion: 0.09
Nodes (22): Primitives Built Lazily, Mesh3D, shared_ptr, vector, string, vec2, vec3, Mesh3D (+14 more)

### Community 17 - "Community 17"
Cohesion: 0.11
Nodes (20): B7: No Serialization, D5: YAML Scene Serialization, Headless Self-Tests, UUIDs, Not Entity Handles, YAML Scene Files, Emitter, convert<glm::vec3>, convert<glm::vec4> (+12 more)

### Community 18 - "Community 18"
Cohesion: 0.12
Nodes (17): D6: Dear ImGui Editor, Offscreen Scene Target, Viewport Samples the Framebuffer, Framebuffer, bind, colorAttachment, create, depthAttachment (+9 more)

### Community 19 - "Community 19"
Cohesion: 0.16
Nodes (19): Script Failures Are Isolated, onUpdateRuntime, Scene, nativeGetRotation(), nativeGetScale(), nativeGetTranslation(), nativeSetRotation(), nativeSetScale() (+11 more)

### Community 20 - "Community 20"
Cohesion: 0.10
Nodes (21): Palette, Accent, AccentDim, AccentFaint, AccentHover, AxisX, AxisY, AxisZ (+13 more)

### Community 21 - "Community 21"
Cohesion: 0.15
Nodes (20): GameObject, string, GameObject, components, componentsJustAdded, Create, destroy, flaggedForDeletion (+12 more)

### Community 22 - "Community 22"
Cohesion: 0.10
Nodes (21): Button, boxOpacity, _color, _hoverColor, isMouseOver, _isPressed, onClick, OnCursorEntered (+13 more)

### Community 23 - "Community 23"
Cohesion: 0.17
Nodes (18): Cook-Torrance GGX over Metallic-Roughness, Inverse-Transpose Normal Matrix, array, mat4, Mesh3D, shared_ptr, vec3, Renderer3D (+10 more)

### Community 24 - "Community 24"
Cohesion: 0.15
Nodes (17): string, GameObject, string, unique_ptr, LoadScene, gameObject, load, _loadTime (+9 more)

### Community 25 - "Community 25"
Cohesion: 0.10
Nodes (20): string, Texture2D, vec2, vec3, SpriteRenderer, awake, _color, getTexture (+12 more)

### Community 26 - "Community 26"
Cohesion: 0.15
Nodes (16): copyComponentType(), entity, string, unordered_map, Entity, string, unordered_map, Scene (+8 more)

### Community 27 - "Community 27"
Cohesion: 0.16
Nodes (16): B6: 2D-Only Transform, Copy-on-Play, D3: Unified 3D Transform, 3D Lays Down Depth, Sprites Blend Over, entt Confined to Entity and Scene, EnTT Confined to Entity.h, Forward, Not Deferred, ScriptEngine Binds Managed Types to Entities (+8 more)

### Community 28 - "Community 28"
Cohesion: 0.21
Nodes (7): vec2, Graphics, drawMesh, Texture2D, vec4, GameObject, onRender

### Community 29 - "Community 29"
Cohesion: 0.15
Nodes (12): function, function, vector, Scheduler, addDelayedTask, addRepeatingTask, delayedTasks, dispose (+4 more)

### Community 30 - "Community 30"
Cohesion: 0.14
Nodes (7): connect_member(), function, T, Signal, _current_id, _slots, map

### Community 31 - "Community 31"
Cohesion: 0.16
Nodes (13): GameObject, string, GameObjectCollection, add, find, gameObjectMap, gameObjects, gameObjectsToAdd (+5 more)

### Community 32 - "Community 32"
Cohesion: 0.12
Nodes (15): string, loadScene, GameObject, string, Texture2D, unique_ptr, SplashScreenScene, gameObject (+7 more)

### Community 33 - "Community 33"
Cohesion: 0.14
Nodes (13): ScrapGameEngine::Button::render(), vec4, HoverSensor, boxOpacity, halfHeight, halfWidth, height, isHovered (+5 more)

### Community 34 - "Community 34"
Cohesion: 0.25
Nodes (12): char_t_str, Blittable Function-Pointer Interop, string, DotNetHost, getFunction, initialize, isReady, shutdown (+4 more)

### Community 35 - "Community 35"
Cohesion: 0.22
Nodes (12): GLFWwindow, ImGuiLayer, begin, beginDockspace, end, endDockspace, init, shutdown (+4 more)

### Community 36 - "Community 36"
Cohesion: 0.15
Nodes (12): drawQuad, GameObject, Texture2D, vec2, vec3, ScrapGameEngine::SpriteRenderer::getTexture(), ScrapGameEngine::SpriteRenderer::render(), ScrapGameEngine::SpriteRenderer::setColour() (+4 more)

### Community 37 - "Community 37"
Cohesion: 0.13
Nodes (15): Renderer3DData, cameraPosition, gridShader, gridVao, gridVbo, gridVertexCount, initialized, pointLightCount (+7 more)

### Community 38 - "Community 38"
Cohesion: 0.19
Nodes (11): ShaderDataType, toGLBaseType(), VertexArray, addVertexBuffer, attributeIndex, bind, create, id (+3 more)

### Community 39 - "Community 39"
Cohesion: 0.23
Nodes (14): BaseScene, activate, deactivate, getName, initialize, onActivate, onDeactivate, onInitialize (+6 more)

### Community 40 - "Community 40"
Cohesion: 0.13
Nodes (14): entt, freetype, glfw3, glm, imguizmo, miniaudio, yaml-cpp, builtin-baseline (+6 more)

### Community 41 - "Community 41"
Cohesion: 0.23
Nodes (11): BaseComponent, awake, destroy, flaggedForDeletion, gameObject, render, shouldDestroy, start (+3 more)

### Community 42 - "Community 42"
Cohesion: 0.23
Nodes (9): Component, float, int, UnmanagedCallersOnly, Bootstrap, Component, Rigidbody, Sprite (+1 more)

### Community 43 - "Community 43"
Cohesion: 0.17
Nodes (11): update, GameObject, string, Texture2D, unique_ptr, MainMenuScene, gameObject, onRender (+3 more)

### Community 44 - "Community 44"
Cohesion: 0.19
Nodes (10): AllocatedTexture, refCount, texture, string, Texture2D, unordered_map, TextureAllocator, cacheMutex (+2 more)

### Community 45 - "Community 45"
Cohesion: 0.21
Nodes (11): vec3, Panels::drawConsole(), Panels::drawContentBrowser(), Panels::drawHierarchy(), Panels::drawInspector(), Panels::drawToolbar(), Panels::drawViewport(), Panels::setContentRoot() (+3 more)

### Community 46 - "Community 46"
Cohesion: 0.17
Nodes (11): vec4, Material, albedo, emissive, metallic, roughness, Mesh3D, Renderer3DStats (+3 more)

### Community 47 - "Community 47"
Cohesion: 0.21
Nodes (11): string, Mesh3D, ScriptComponent, typeName, SpriteRendererComponent, color, texture, texturePath (+3 more)

### Community 48 - "Community 48"
Cohesion: 0.21
Nodes (9): GameObject, vec2, vec4, ScrapGameEngine::Button::Button(), ScrapGameEngine::Button::setColor(), ScrapGameEngine::Button::setColour(), ScrapGameEngine::Button::setHoverColor(), ScrapGameEngine::Button::setHoverColour() (+1 more)

### Community 49 - "Community 49"
Cohesion: 0.18
Nodes (11): shared_ptr, vec4, MeshRendererComponent, albedo, emissive, mesh, meshPath, metallic (+3 more)

### Community 50 - "Community 50"
Cohesion: 0.20
Nodes (10): DirectionalLight, color, direction, intensity, vec3, PointLight, color, intensity (+2 more)

### Community 51 - "Community 51"
Cohesion: 0.31
Nodes (9): string, Texture2D, getTexture, releaseUnusedTextures, returnTexture, onDeactivate, onDeactivate, onDeactivate (+1 more)

### Community 52 - "Community 52"
Cohesion: 0.22
Nodes (6): deque, Entity, Level, LogEntry, level, message

### Community 53 - "Community 53"
Cohesion: 0.22
Nodes (8): B8: dynamic_cast Component Lookup, Batched Quad Renderer, GameObjectCollection Was Never Populated, CoreCLR over NativeAOT, D10 revised: C# on CoreCLR, D1: EnTT Entity Model, Entity Is a Field, Not a Property, SpriteRenderer Pointers Were Uninitialised

### Community 54 - "Community 54"
Cohesion: 0.33
Nodes (7): The Editor Owns Its Loop, Scene, shared_ptr, createStarterScene(), main(), runScriptTest(), runSelfTest()

### Community 55 - "Community 55"
Cohesion: 0.32
Nodes (4): Level, string, log, string

### Community 56 - "Community 56"
Cohesion: 0.25
Nodes (8): CameraComponent, farClip, fovDegrees, nearClip, orthoSize, primary, projection, ProjectionKind

### Community 57 - "Community 57"
Cohesion: 0.32
Nodes (4): Entity, handle, scene, Scene

### Community 58 - "Community 58"
Cohesion: 0.33
Nodes (6): mat4, vec3, TransformComponent, rotation, scale, translation

### Community 59 - "Community 59"
Cohesion: 0.29
Nodes (4): IDComponent, id, UUID, value

### Community 60 - "Community 60"
Cohesion: 0.33
Nodes (3): Scene, T, tryGetComponent()

### Community 61 - "Community 61"
Cohesion: 0.33
Nodes (3): unordered_map, addScene(), T

### Community 62 - "Community 62"
Cohesion: 0.33
Nodes (6): LightComponent, color, intensity, kind, range, LightKind

### Community 63 - "Community 63"
Cohesion: 0.50
Nodes (4): addComponent(), getComponent(), T, Transform

### Community 67 - "Community 67"
Cohesion: 0.50
Nodes (3): Entity, Scene, ScriptInterop

### Community 68 - "Community 68"
Cohesion: 0.67
Nodes (3): CI Naming Gate, B12: Coursework Naming, Strangler-Fig Migration

### Community 69 - "Community 69"
Cohesion: 0.67
Nodes (3): Scene, shared_ptr, copy

## Knowledge Gaps
- **384 isolated node(s):** `view`, `projection`, `position`, `focalPoint`, `pitch` (+379 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **4 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `BaseComponent` connect `Community 41` to `Audio Device and Sources`, `Text and Glyphs`, `Transform Hierarchy`, `Tweening and Easing`, `Community 21`, `Community 22`, `Community 55`, `Community 25`, `Community 28`, `Community 29`, `Community 30`, `Community 63`?**
  _High betweenness centrality (0.127) - this node is a cross-community bridge._
- **Why does `B8: dynamic_cast Component Lookup` connect `Community 53` to `Community 41`, `Community 27`?**
  _High betweenness centrality (0.111) - this node is a cross-community bridge._
- **What connects `view`, `projection`, `position` to the rest of the system?**
  _384 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Render Frame and Targets` be split into smaller, more focused modules?**
  _Cohesion score 0.055218855218855216 - nodes in this community are weakly interconnected._
- **Should `C# Scripting API` be split into smaller, more focused modules?**
  _Cohesion score 0.0660377358490566 - nodes in this community are weakly interconnected._
- **Should `Batched Quad Renderer` be split into smaller, more focused modules?**
  _Cohesion score 0.06547619047619048 - nodes in this community are weakly interconnected._
- **Should `Audio Device and Sources` be split into smaller, more focused modules?**
  _Cohesion score 0.06956521739130435 - nodes in this community are weakly interconnected._