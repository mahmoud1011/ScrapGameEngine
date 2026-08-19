# Graph Report - scrap-engine-system-design-c7c2af  (2026-08-20)

## Corpus Check
- 78 files · ~195,547 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 868 nodes · 1407 edges · 36 communities (34 shown, 2 thin omitted)
- Extraction: 95% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 67 edges (avg confidence: 0.81)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- Renderer Foundations and Platform Targets
- Graphics Submission API
- Batched Quad Renderer
- Camera Statics and Their Replacement
- Application Loop
- Entity Model and Serialization
- Signal Event System
- Build System and Dependencies
- Texture Resources
- Text and Glyph Rendering
- Scene Lifecycle
- Transform Hierarchy
- Task Scheduler
- Tweening and Easing
- Input Handling
- Shader Compilation
- GameObject Composition
- Load Scene
- Sprite Component State
- Framebuffer Targets
- Vertex and Index Buffers
- Game Scene Audio
- Vertex Array Wiring
- Buffer Layout
- Sprite Rendering
- Texture Allocation Cache
- Splash Screen Scene
- vcpkg Dependency Manifest
- Texture Allocator
- Buffer Elements
- Shader Data Types
- Naming Cleanup and CI Gate
- Community 33
- Community 34

## God Nodes (most connected - your core abstractions)
1. `Camera` - 31 edges
2. `Text` - 27 edges
3. `BaseComponent` - 26 edges
4. `Renderer2DData` - 25 edges
5. `BaseScene` - 24 edges
6. `TweenComponent` - 23 edges
7. `Button` - 23 edges
8. `AudioSource` - 22 edges
9. `SpriteRenderer` - 22 edges
10. `Shader` - 22 edges

## Surprising Connections (you probably didn't know these)
- `main()` --calls--> `run`  [INFERRED]
  sandbox/src/main.cpp → engine/src/core/Application.h
- `onInitialize` --calls--> `startTween`  [INFERRED]
  sandbox/src/SplashScreenScene.h → engine/src/scene/TweenComponent.h
- `CI Cross-Platform Build Matrix` --conceptually_related_to--> `OpenGL 3.3 Core Baseline`  [INFERRED]
  .github/workflows/ci.yml → docs/ENGINE_IMPROVEMENT_PLAN.md
- `Desktop-First Platform Targets` --conceptually_related_to--> `D8: miniaudio over irrKlang`  [INFERRED]
  README.md → docs/ENGINE_IMPROVEMENT_PLAN.md
- `GameScene` --references--> `AudioSource`  [EXTRACTED]
  sandbox/src/GameScene.h → engine/src/audio/AudioSource.h

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Editor Prerequisite Chain** — docs_engine_improvement_plan_b1_fixed_function_gl, docs_engine_improvement_plan_d2_rhi_two_renderers, docs_engine_improvement_plan_d2a_opengl_33_core, docs_engine_improvement_plan_d6_imgui_editor, docs_engine_improvement_plan_b4_static_god_classes [INFERRED 0.85]
- **Why It Could Not Build** — docs_engine_improvement_plan_b2_absolute_include_paths, docs_engine_improvement_plan_b3_missing_link_libraries, docs_engine_improvement_plan_b13_msbuild_only, docs_engine_improvement_plan_d7_cmake_vcpkg [EXTRACTED 1.00]
- **Cross-Platform Constraint Set** — readme_platform_targets, docs_engine_improvement_plan_d2a_opengl_33_core, docs_engine_improvement_plan_d8_miniaudio, _github_workflows_ci_matrix_build, docs_engine_improvement_plan_d7_cmake_vcpkg [INFERRED 0.85]
- **Phase 1 Shader Pipeline** — engine_src_rhi_shader_cpp_engine_src_rhi_shader, engine_src_rhi_buffer_cpp_engine_src_rhi_buffer, engine_src_rhi_vertexarray_cpp_engine_src_rhi_vertexarray, engine_src_renderer_renderer2d_cpp_engine_src_renderer_renderer2d, docs_engine_improvement_plan_batched_quad_renderer [EXTRACTED 1.00]
- **What Makes an Editor Viewport Possible** — engine_src_rhi_framebuffer_cpp_engine_src_rhi_framebuffer, docs_engine_improvement_plan_offscreen_scene_target, docs_engine_improvement_plan_d6_imgui_editor, docs_engine_improvement_plan_d4_camera_as_component, docs_engine_improvement_plan_b4_static_god_classes [INFERRED 0.85]

## Communities (36 total, 2 thin omitted)

### Community 0 - "Renderer Foundations and Platform Targets"
Cohesion: 0.05
Nodes (50): CI Cross-Platform Build Matrix, B1: Fixed-Function OpenGL, D2: One RHI, Two Renderers, OpenGL 3.3 Core Baseline, D6: Dear ImGui Editor, glad Regenerated for 3.3 Core, Offscreen Scene Target, run (+42 more)

### Community 1 - "Graphics Submission API"
Cohesion: 0.06
Nodes (33): vec3, Graphics, drawMesh, drawQuad, Texture2D, vector, unique_ptr, vec2 (+25 more)

### Community 2 - "Batched Quad Renderer"
Cohesion: 0.07
Nodes (44): array, Switch-Indexed Sampler Array, White Texture in Slot 0, mat4, Texture2D, vec2, vec3, vec4 (+36 more)

### Community 3 - "Camera Statics and Their Replacement"
Cohesion: 0.08
Nodes (44): B4: Static God Classes, D4: Camera as Component, Camera, aspectRatio, config, getAspectRatio, getMatrix_projection, getMatrix_view (+36 more)

### Community 4 - "Application Loop"
Cohesion: 0.06
Nodes (37): AppWindowEventType, B10: Busy-Wait Frame Limiter, B11: Member Initialisation Order Bug, shutdown, Application, cleanup, frameTime, init (+29 more)

### Community 5 - "Entity Model and Serialization"
Cohesion: 0.06
Nodes (38): B7: No Serialization, B8: dynamic_cast Component Lookup, Batched Quad Renderer, D1: EnTT Entity Model, D5: YAML Scene Serialization, string, BaseComponent, awake (+30 more)

### Community 6 - "Signal Event System"
Cohesion: 0.06
Nodes (29): connect_member(), function, T, Signal, _current_id, _slots, GameObject, vec2 (+21 more)

### Community 7 - "Build System and Dependencies"
Cohesion: 0.08
Nodes (38): scrap_warnings Interface Target, ScrapEngine CMake Project, Shared AudioDevice, B13: MSBuild Only, No CI, B2: Absolute Include Paths, B3: Missing Link Libraries, B5: Game Compiled Into Engine, D7: CMake and vcpkg (+30 more)

### Community 8 - "Texture Resources"
Cohesion: 0.09
Nodes (36): string, Texture2D, TextureFilterMode, TextureWrapMode, string, TextureFilterMode, TextureWrapMode, vec4 (+28 more)

### Community 9 - "Text and Glyph Rendering"
Cohesion: 0.08
Nodes (33): vec4, GameObject, string, Glyph, advance, offset, size, textureCoords (+25 more)

### Community 10 - "Scene Lifecycle"
Cohesion: 0.09
Nodes (29): BaseScene, activate, deactivate, getName, initialize, onActivate, onDeactivate, onInitialize (+21 more)

### Community 11 - "Transform Hierarchy"
Cohesion: 0.09
Nodes (30): GameObject, vec2, vec2, vector, ScrapGameEngine::Transform::calculateWorldScale(), ScrapGameEngine::Transform::getLocalScale(), ScrapGameEngine::Transform::getParent(), ScrapGameEngine::Transform::getPosition() (+22 more)

### Community 12 - "Task Scheduler"
Cohesion: 0.09
Nodes (23): function, function, vector, Scheduler, addDelayedTask, addRepeatingTask, delayedTasks, dispose (+15 more)

### Community 13 - "Tweening and Easing"
Cohesion: 0.09
Nodes (29): EasingType, function, GameObject, TweenType, vec3, getEasingFunction(), EasingType, function (+21 more)

### Community 14 - "Input Handling"
Cohesion: 0.12
Nodes (25): KeyCode, MouseButtonCode, vec2, KeyCode, MouseButtonCode, unordered_map, Input, getKey (+17 more)

### Community 15 - "Shader Compilation"
Cohesion: 0.17
Nodes (22): compileStage(), mat4, string, vec2, vec3, vec4, string, unordered_map (+14 more)

### Community 16 - "GameObject Composition"
Cohesion: 0.12
Nodes (24): addComponent(), GameObject, string, GameObject, components, componentsJustAdded, Create, destroy (+16 more)

### Community 17 - "Load Scene"
Cohesion: 0.15
Nodes (17): string, GameObject, string, unique_ptr, LoadScene, gameObject, load, _loadTime (+9 more)

### Community 18 - "Sprite Component State"
Cohesion: 0.10
Nodes (20): string, Texture2D, vec2, vec3, SpriteRenderer, awake, _color, getTexture (+12 more)

### Community 19 - "Framebuffer Targets"
Cohesion: 0.15
Nodes (14): Framebuffer, bind, colorAttachment, create, depthAttachment, destroy, id, resize (+6 more)

### Community 20 - "Vertex and Index Buffers"
Cohesion: 0.15
Nodes (14): IndexBuffer, bind, count, create, id, unbind, VertexBuffer, bind (+6 more)

### Community 21 - "Game Scene Audio"
Cohesion: 0.13
Nodes (17): GameScene, applyAudioSet, buttonAudioMap, clickAudioSource, initializeAudioSets, musicPadButtons, onInitialize, onRender (+9 more)

### Community 22 - "Vertex Array Wiring"
Cohesion: 0.20
Nodes (10): Layout-Driven Vertex Attributes, RHI Abstraction Layer, VertexArray, attributeIndex, bind, create, id, indexCount (+2 more)

### Community 23 - "Buffer Layout"
Cohesion: 0.18
Nodes (7): const_iterator, BufferLayout, calculateOffsetsAndStride, elements, stride, vector, initializer_list

### Community 24 - "Sprite Rendering"
Cohesion: 0.18
Nodes (10): GameObject, Texture2D, vec2, vec3, ScrapGameEngine::SpriteRenderer::getTexture(), ScrapGameEngine::SpriteRenderer::setColour(), ScrapGameEngine::SpriteRenderer::setPivot(), ScrapGameEngine::SpriteRenderer::setSize() (+2 more)

### Community 25 - "Texture Allocation Cache"
Cohesion: 0.19
Nodes (10): AllocatedTexture, refCount, texture, string, Texture2D, unordered_map, TextureAllocator, cacheMutex (+2 more)

### Community 26 - "Splash Screen Scene"
Cohesion: 0.15
Nodes (12): GameObject, string, Texture2D, unique_ptr, SplashScreenScene, gameObject, gameObject1, _mesh (+4 more)

### Community 27 - "vcpkg Dependency Manifest"
Cohesion: 0.17
Nodes (11): freetype, glfw3, glm, miniaudio, builtin-baseline, dependencies, description, homepage (+3 more)

### Community 28 - "Texture Allocator"
Cohesion: 0.31
Nodes (9): string, Texture2D, getTexture, releaseUnusedTextures, returnTexture, onDeactivate, onDeactivate, onDeactivate (+1 more)

### Community 29 - "Buffer Elements"
Cohesion: 0.28
Nodes (8): BufferElement, name, normalized, offset, size, type, ShaderDataType, string

### Community 30 - "Shader Data Types"
Cohesion: 0.29
Nodes (7): ShaderDataType, shaderDataTypeComponentCount(), shaderDataTypeSize(), ShaderDataType, toGLBaseType(), addVertexBuffer, GLenum

### Community 32 - "Naming Cleanup and CI Gate"
Cohesion: 0.67
Nodes (3): CI Naming Gate, B12: Coursework Naming, Strangler-Fig Migration

## Knowledge Gaps
- **248 isolated node(s):** `sound`, `filePath`, `looping`, `volume`, `instance` (+243 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **2 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `BaseComponent` connect `Entity Model and Serialization` to `Graphics Submission API`, `Build System and Dependencies`, `Text and Glyph Rendering`, `Transform Hierarchy`, `Task Scheduler`, `Tweening and Easing`, `GameObject Composition`, `Sprite Component State`?**
  _High betweenness centrality (0.175) - this node is a cross-community bridge._
- **Why does `AudioSource` connect `Build System and Dependencies` to `Graphics Submission API`, `Game Scene Audio`, `Task Scheduler`, `Entity Model and Serialization`?**
  _High betweenness centrality (0.085) - this node is a cross-community bridge._
- **Why does `BaseScene` connect `Scene Lifecycle` to `Renderer Foundations and Platform Targets`, `Graphics Submission API`, `Task Scheduler`, `Load Scene`, `Game Scene Audio`, `Splash Screen Scene`?**
  _High betweenness centrality (0.079) - this node is a cross-community bridge._
- **What connects `sound`, `filePath`, `looping` to the rest of the system?**
  _248 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Renderer Foundations and Platform Targets` be split into smaller, more focused modules?**
  _Cohesion score 0.05454545454545454 - nodes in this community are weakly interconnected._
- **Should `Graphics Submission API` be split into smaller, more focused modules?**
  _Cohesion score 0.05959183673469388 - nodes in this community are weakly interconnected._
- **Should `Batched Quad Renderer` be split into smaller, more focused modules?**
  _Cohesion score 0.06547619047619048 - nodes in this community are weakly interconnected._