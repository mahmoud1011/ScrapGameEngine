# Graph Report - scrap-engine-system-design-c7c2af  (2026-08-20)

## Corpus Check
- 94 files · ~203,156 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 1048 nodes · 1700 edges · 56 communities (50 shown, 6 thin omitted)
- Extraction: 95% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 77 edges (avg confidence: 0.81)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- Community 0
- Community 1
- Community 2
- Community 3
- Community 4
- Community 5
- Community 6
- Community 7
- Community 8
- Community 9
- Community 10
- Community 11
- Community 12
- Community 13
- Community 14
- Community 15
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
- Community 49
- Community 50
- Community 51
- Community 52
- Community 53
- Community 54

## God Nodes (most connected - your core abstractions)
1. `EditorContext` - 44 edges
2. `EditorCamera` - 34 edges
3. `Camera` - 31 edges
4. `Text` - 27 edges
5. `BaseComponent` - 26 edges
6. `Renderer2DData` - 25 edges
7. `BaseScene` - 24 edges
8. `main()` - 23 edges
9. `TweenComponent` - 23 edges
10. `Button` - 23 edges

## Surprising Connections (you probably didn't know these)
- `main()` --calls--> `run`  [INFERRED]
  sandbox/src/main.cpp → engine/src/core/Application.h
- `onInitialize` --calls--> `startTween`  [INFERRED]
  sandbox/src/SplashScreenScene.h → engine/src/scene/TweenComponent.h
- `CI Cross-Platform Build Matrix` --conceptually_related_to--> `OpenGL 3.3 Core Baseline`  [INFERRED]
  .github/workflows/ci.yml → docs/ENGINE_IMPROVEMENT_PLAN.md
- `Desktop-First Platform Targets` --conceptually_related_to--> `D8: miniaudio over irrKlang`  [INFERRED]
  README.md → docs/ENGINE_IMPROVEMENT_PLAN.md
- `Panels::drawViewport()` --references--> `Renderer`  [EXTRACTED]
  editor/src/Panels.cpp → engine/src/renderer/Renderer.h

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **What Made the Editor Possible** — editor_src_imguilayer_cpp_editor_src_imguilayer, editor_src_panels_cpp_editor_src_panels, docs_engine_improvement_plan_viewport_samples_fbo, docs_engine_improvement_plan_offscreen_scene_target, editor_src_editorcamera_cpp_editor_src_editorcamera [EXTRACTED 1.00]
- **Managed Scripting Stack** — engine_src_scripting_dotnethost_cpp_engine_src_scripting_dotnethost, docs_engine_improvement_plan_d10_coreclr_scripting, docs_engine_improvement_plan_coreclr_over_nativeaot, docs_engine_improvement_plan_blittable_interop_boundary, docs_engine_improvement_plan_optional_dotnet_dependency [EXTRACTED 1.00]
- **Why It Could Not Build** — docs_engine_improvement_plan_b2_absolute_include_paths, docs_engine_improvement_plan_b3_missing_link_libraries, docs_engine_improvement_plan_b13_msbuild_only, docs_engine_improvement_plan_d7_cmake_vcpkg [EXTRACTED 1.00]
- **Phase 1 Shader Pipeline** — engine_src_rhi_shader_cpp_engine_src_rhi_shader, engine_src_rhi_buffer_cpp_engine_src_rhi_buffer, engine_src_rhi_vertexarray_cpp_engine_src_rhi_vertexarray, engine_src_renderer_renderer2d_cpp_engine_src_renderer_renderer2d, docs_engine_improvement_plan_batched_quad_renderer [EXTRACTED 1.00]
- **Cross-Platform Constraint Set** — readme_platform_targets, docs_engine_improvement_plan_d2a_opengl_33_core, docs_engine_improvement_plan_d8_miniaudio, _github_workflows_ci_matrix_build, docs_engine_improvement_plan_d7_cmake_vcpkg [INFERRED 0.85]
- **Editor Prerequisite Chain** — docs_engine_improvement_plan_b1_fixed_function_gl, docs_engine_improvement_plan_d2_rhi_two_renderers, docs_engine_improvement_plan_d2a_opengl_33_core, docs_engine_improvement_plan_d6_imgui_editor, docs_engine_improvement_plan_b4_static_god_classes [INFERRED 0.85]
- **What Makes an Editor Viewport Possible** — engine_src_rhi_framebuffer_cpp_engine_src_rhi_framebuffer, docs_engine_improvement_plan_offscreen_scene_target, docs_engine_improvement_plan_d6_imgui_editor, docs_engine_improvement_plan_d4_camera_as_component, docs_engine_improvement_plan_b4_static_god_classes [INFERRED 0.85]
- **Latent Bugs Found Only by Using the Code** — docs_engine_improvement_plan_uninitialised_sprite_pointers, docs_engine_improvement_plan_collection_was_dead_code, docs_engine_improvement_plan_b11_member_init_order, docs_engine_improvement_plan_time_h_shadows_ctime [INFERRED 0.85]

## Communities (56 total, 6 thin omitted)

### Community 0 - "Community 0"
Cohesion: 0.05
Nodes (49): char_t_str, Blittable Function-Pointer Interop, The Editor Owns Its Loop, ScrapEngine Editor Identity, GLFWwindow, ImGuiLayer, begin, beginDockspace (+41 more)

### Community 1 - "Community 1"
Cohesion: 0.06
Nodes (47): CI Cross-Platform Build Matrix, B1: Fixed-Function OpenGL, D2: One RHI, Two Renderers, OpenGL 3.3 Core Baseline, D6: Dear ImGui Editor, glad Regenerated for 3.3 Core, Offscreen Scene Target, RHI Abstraction Layer (+39 more)

### Community 2 - "Community 2"
Cohesion: 0.07
Nodes (44): array, Switch-Indexed Sampler Array, White Texture in Slot 0, mat4, Texture2D, vec2, vec3, vec4 (+36 more)

### Community 3 - "Community 3"
Cohesion: 0.06
Nodes (41): KeyCode, MouseButtonCode, vec2, KeyCode, MouseButtonCode, unordered_map, Input, getKey (+33 more)

### Community 4 - "Community 4"
Cohesion: 0.06
Nodes (37): AppWindowEventType, B10: Busy-Wait Frame Limiter, B11: Member Initialisation Order Bug, shutdown, Application, cleanup, frameTime, init (+29 more)

### Community 5 - "Community 5"
Cohesion: 0.08
Nodes (42): Camera, aspectRatio, config, getAspectRatio, getMatrix_projection, getMatrix_view, getMatrix_viewProjection, getOrthoSize (+34 more)

### Community 6 - "Community 6"
Cohesion: 0.08
Nodes (38): scrap_warnings Interface Target, ScrapEngine CMake Project, Shared AudioDevice, B13: MSBuild Only, No CI, B2: Absolute Include Paths, B3: Missing Link Libraries, D7: CMake and vcpkg, D8: miniaudio over irrKlang (+30 more)

### Community 7 - "Community 7"
Cohesion: 0.06
Nodes (39): B7: No Serialization, B8: dynamic_cast Component Lookup, Batched Quad Renderer, CoreCLR over NativeAOT, D10 revised: C# on CoreCLR, D1: EnTT Entity Model, D5: YAML Scene Serialization, SpriteRenderer Pointers Were Uninitialised (+31 more)

### Community 8 - "Community 8"
Cohesion: 0.10
Nodes (35): string, Texture2D, TextureFilterMode, TextureWrapMode, string, TextureFilterMode, TextureWrapMode, vec4 (+27 more)

### Community 9 - "Community 9"
Cohesion: 0.08
Nodes (32): GameObject, string, Glyph, advance, offset, size, textureCoords, string (+24 more)

### Community 10 - "Community 10"
Cohesion: 0.08
Nodes (27): B4: Static God Classes, D4: Camera as Component, vec2, EditorCamera, dirty, farClip, focalPoint, fovDegrees (+19 more)

### Community 11 - "Community 11"
Cohesion: 0.09
Nodes (30): GameObject, vec2, vec2, vector, ScrapGameEngine::Transform::calculateWorldScale(), ScrapGameEngine::Transform::getLocalScale(), ScrapGameEngine::Transform::getParent(), ScrapGameEngine::Transform::getPosition() (+22 more)

### Community 12 - "Community 12"
Cohesion: 0.10
Nodes (27): BaseScene, activate, deactivate, getName, initialize, onActivate, onDeactivate, onInitialize (+19 more)

### Community 13 - "Community 13"
Cohesion: 0.09
Nodes (29): EasingType, function, GameObject, TweenType, vec3, getEasingFunction(), EasingType, function (+21 more)

### Community 14 - "Community 14"
Cohesion: 0.07
Nodes (22): EditorContext, camera, entries, fps, frameMs, gizmoLocalSpace, gizmoOp, kMaxLogEntries (+14 more)

### Community 15 - "Community 15"
Cohesion: 0.17
Nodes (22): compileStage(), mat4, string, vec2, vec3, vec4, string, unordered_map (+14 more)

### Community 16 - "Community 16"
Cohesion: 0.12
Nodes (24): addComponent(), GameObject, string, GameObject, components, componentsJustAdded, Create, destroy (+16 more)

### Community 17 - "Community 17"
Cohesion: 0.10
Nodes (21): Button, boxOpacity, _color, _hoverColor, isMouseOver, _isPressed, onClick, OnCursorEntered (+13 more)

### Community 18 - "Community 18"
Cohesion: 0.15
Nodes (17): string, GameObject, string, unique_ptr, LoadScene, gameObject, load, _loadTime (+9 more)

### Community 19 - "Community 19"
Cohesion: 0.15
Nodes (14): Framebuffer, bind, colorAttachment, create, depthAttachment, destroy, id, resize (+6 more)

### Community 20 - "Community 20"
Cohesion: 0.17
Nodes (13): Layout-Driven Vertex Attributes, ShaderDataType, toGLBaseType(), VertexArray, addVertexBuffer, attributeIndex, bind, create (+5 more)

### Community 21 - "Community 21"
Cohesion: 0.15
Nodes (7): connect_member(), function, T, Signal, _current_id, _slots, map

### Community 22 - "Community 22"
Cohesion: 0.14
Nodes (13): ScrapGameEngine::Button::render(), vec4, HoverSensor, boxOpacity, halfHeight, halfWidth, height, isHovered (+5 more)

### Community 23 - "Community 23"
Cohesion: 0.19
Nodes (10): Component, Scrap, float, int, Bootstrap, Component, Rigidbody, Sprite (+2 more)

### Community 24 - "Community 24"
Cohesion: 0.19
Nodes (12): ShaderDataType, create, shaderDataTypeComponentCount(), shaderDataTypeSize(), VertexBuffer, bind, createDynamic, createStatic (+4 more)

### Community 25 - "Community 25"
Cohesion: 0.22
Nodes (12): vec3, Panels::drawConsole(), Panels::drawContentBrowser(), Panels::drawHierarchy(), Panels::drawInspector(), Panels::drawToolbar(), Panels::drawViewport(), Panels::setContentRoot() (+4 more)

### Community 26 - "Community 26"
Cohesion: 0.17
Nodes (11): update, GameObject, string, Texture2D, unique_ptr, MainMenuScene, gameObject, onRender (+3 more)

### Community 27 - "Community 27"
Cohesion: 0.19
Nodes (11): vec3, vec4, RenderParams, rotationZ, scale, texture, tint, translation (+3 more)

### Community 28 - "Community 28"
Cohesion: 0.18
Nodes (10): GameObject, Texture2D, vec2, vec3, ScrapGameEngine::SpriteRenderer::getTexture(), ScrapGameEngine::SpriteRenderer::setColour(), ScrapGameEngine::SpriteRenderer::setPivot(), ScrapGameEngine::SpriteRenderer::setSize() (+2 more)

### Community 29 - "Community 29"
Cohesion: 0.19
Nodes (10): AllocatedTexture, refCount, texture, string, Texture2D, unordered_map, TextureAllocator, cacheMutex (+2 more)

### Community 30 - "Community 30"
Cohesion: 0.19
Nodes (9): GameObject, vec2, vec4, ScrapGameEngine::Button::Button(), ScrapGameEngine::Button::setColor(), ScrapGameEngine::Button::setColour(), ScrapGameEngine::Button::setHoverColor(), ScrapGameEngine::Button::setHoverColour() (+1 more)

### Community 31 - "Community 31"
Cohesion: 0.15
Nodes (12): freetype, glfw3, glm, imguizmo, miniaudio, builtin-baseline, dependencies, description (+4 more)

### Community 32 - "Community 32"
Cohesion: 0.15
Nodes (12): GameObject, string, Texture2D, unique_ptr, SplashScreenScene, gameObject, gameObject1, _mesh (+4 more)

### Community 33 - "Community 33"
Cohesion: 0.20
Nodes (7): const_iterator, BufferLayout, calculateOffsetsAndStride, elements, stride, vector, initializer_list

### Community 34 - "Community 34"
Cohesion: 0.17
Nodes (9): vector, unique_ptr, Mesh, getID, getVertexCount, id, meshData, vertexCount (+1 more)

### Community 35 - "Community 35"
Cohesion: 0.36
Nodes (3): vec2, Texture2D, GameObject

### Community 36 - "Community 36"
Cohesion: 0.24
Nodes (10): function, function, Scheduler, addDelayedTask, addRepeatingTask, delayedTasks, dispose, repeatingTasks (+2 more)

### Community 37 - "Community 37"
Cohesion: 0.31
Nodes (9): string, Texture2D, getTexture, releaseUnusedTextures, returnTexture, onDeactivate, onDeactivate, onDeactivate (+1 more)

### Community 38 - "Community 38"
Cohesion: 0.22
Nodes (5): deque, B5: Game Compiled Into Engine, EditorContext Keeps Editor State Out of the Engine, Engine Included a Game Scene, GameObject

### Community 39 - "Community 39"
Cohesion: 0.22
Nodes (8): vec2, vec3, Vertex, u, v, x, y, z

### Community 40 - "Community 40"
Cohesion: 0.31
Nodes (8): vector, unordered_map, hashVertices(), MeshAllocator, getMesh, meshCache, releaseUnusedMeshes, shared_ptr

### Community 41 - "Community 41"
Cohesion: 0.28
Nodes (8): BufferElement, name, normalized, offset, size, type, ShaderDataType, string

### Community 42 - "Community 42"
Cohesion: 0.32
Nodes (4): Level, string, log, string

### Community 43 - "Community 43"
Cohesion: 0.25
Nodes (5): IndexBuffer, bind, count, id, unbind

### Community 45 - "Community 45"
Cohesion: 0.33
Nodes (5): Graphics, drawMesh, drawQuad, ScrapGameEngine::SpriteRenderer::render(), onRender

### Community 47 - "Community 47"
Cohesion: 0.50
Nodes (4): Level, LogEntry, level, message

### Community 49 - "Community 49"
Cohesion: 0.67
Nodes (3): CI Naming Gate, B12: Coursework Naming, Strangler-Fig Migration

### Community 50 - "Community 50"
Cohesion: 0.67
Nodes (3): string, loadScene, onUpdate

## Knowledge Gaps
- **308 isolated node(s):** `view`, `projection`, `position`, `focalPoint`, `pitch` (+303 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **6 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `BaseComponent` connect `Community 7` to `Community 35`, `Community 6`, `Community 9`, `Community 42`, `Community 11`, `Community 44`, `Community 13`, `Community 16`, `Community 17`, `Community 21`?**
  _High betweenness centrality (0.143) - this node is a cross-community bridge._
- **Why does `EditorContext` connect `Community 14` to `Community 0`, `Community 1`, `Community 38`, `Community 10`, `Community 42`, `Community 47`, `Community 25`?**
  _High betweenness centrality (0.079) - this node is a cross-community bridge._
- **Why does `AudioSource` connect `Community 6` to `Community 35`, `Community 3`, `Community 7`, `Community 42`, `Community 26`?**
  _High betweenness centrality (0.073) - this node is a cross-community bridge._
- **What connects `view`, `projection`, `position` to the rest of the system?**
  _308 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Community 0` be split into smaller, more focused modules?**
  _Cohesion score 0.05376972530683811 - nodes in this community are weakly interconnected._
- **Should `Community 1` be split into smaller, more focused modules?**
  _Cohesion score 0.06108597285067873 - nodes in this community are weakly interconnected._
- **Should `Community 2` be split into smaller, more focused modules?**
  _Cohesion score 0.06547619047619048 - nodes in this community are weakly interconnected._