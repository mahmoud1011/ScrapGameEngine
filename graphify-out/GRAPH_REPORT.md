# Graph Report - scrap-engine-system-design-c7c2af  (2026-08-20)

## Corpus Check
- 68 files · ~116,057 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 692 nodes · 1087 edges · 35 communities (32 shown, 3 thin omitted)
- Extraction: 96% EXTRACTED · 4% INFERRED · 0% AMBIGUOUS · INFERRED: 42 edges (avg confidence: 0.82)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- Sprite Rendering
- Scene Lifecycle
- Input Handling
- Application Loop
- Build System and Dependencies
- Graphics Draw Parameters
- Texture Allocation
- Text and Glyph Rendering
- Transform Hierarchy
- Tweening and Easing
- GameObject Composition
- Load Scene
- Button Interaction
- Sprite Component State
- Entity Model and Serialization
- Camera State
- GameObject Registry
- Signal Event System
- Hover Sensors
- Camera Statics and Their Replacement
- Renderer and Editor Prerequisites
- Draw Commands
- Button Implementation
- vcpkg Dependency Manifest
- Task Scheduler
- Render Submission
- Public Header Surface
- Camera and Viewport Setup
- Engine Header Includes
- Cross-Platform Targets
- Naming Cleanup and CI Gate
- Transform Dimensionality
- Scripting Direction

## God Nodes (most connected - your core abstractions)
1. `Texture2D` - 34 edges
2. `Text` - 27 edges
3. `BaseComponent` - 26 edges
4. `BaseScene` - 24 edges
5. `TweenComponent` - 23 edges
6. `Button` - 23 edges
7. `AudioSource` - 22 edges
8. `Camera` - 22 edges
9. `SpriteRenderer` - 22 edges
10. `Transform` - 22 edges

## Surprising Connections (you probably didn't know these)
- `main()` --calls--> `run`  [INFERRED]
  sandbox/src/main.cpp → engine/src/core/Application.h
- `onInitialize` --calls--> `startTween`  [INFERRED]
  sandbox/src/SplashScreenScene.h → engine/src/scene/TweenComponent.h
- `Desktop-First Platform Targets` --conceptually_related_to--> `D8: miniaudio over irrKlang`  [INFERRED]
  README.md → docs/ENGINE_IMPROVEMENT_PLAN.md
- `GameScene` --references--> `AudioSource`  [EXTRACTED]
  sandbox/src/GameScene.h → engine/src/audio/AudioSource.h
- `main()` --calls--> `init`  [INFERRED]
  sandbox/src/main.cpp → engine/src/core/Application.h

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Editor Prerequisite Chain** — docs_engine_improvement_plan_b1_fixed_function_gl, docs_engine_improvement_plan_d2_rhi_two_renderers, docs_engine_improvement_plan_d2a_opengl_33_core, docs_engine_improvement_plan_d6_imgui_editor, docs_engine_improvement_plan_b4_static_god_classes [INFERRED 0.85]
- **Why It Could Not Build** — docs_engine_improvement_plan_b2_absolute_include_paths, docs_engine_improvement_plan_b3_missing_link_libraries, docs_engine_improvement_plan_b13_msbuild_only, docs_engine_improvement_plan_d7_cmake_vcpkg [EXTRACTED 1.00]
- **Cross-Platform Constraint Set** — readme_platform_targets, docs_engine_improvement_plan_d2a_opengl_33_core, docs_engine_improvement_plan_d8_miniaudio, _github_workflows_ci_matrix_build, docs_engine_improvement_plan_d7_cmake_vcpkg [INFERRED 0.85]

## Communities (35 total, 3 thin omitted)

### Community 0 - "Sprite Rendering"
Cohesion: 0.07
Nodes (45): GameObject, vec2, vec3, ScrapGameEngine::SpriteRenderer::awake(), ScrapGameEngine::SpriteRenderer::getTexture(), ScrapGameEngine::SpriteRenderer::setColour(), ScrapGameEngine::SpriteRenderer::setPivot(), ScrapGameEngine::SpriteRenderer::setSize() (+37 more)

### Community 1 - "Scene Lifecycle"
Cohesion: 0.07
Nodes (39): update, BaseScene, activate, deactivate, getName, initialize, onActivate, onDeactivate (+31 more)

### Community 2 - "Input Handling"
Cohesion: 0.07
Nodes (39): KeyCode, MouseButtonCode, vec2, KeyCode, MouseButtonCode, unordered_map, Input, getKey (+31 more)

### Community 3 - "Application Loop"
Cohesion: 0.06
Nodes (37): AppWindowEventType, B10: Busy-Wait Frame Limiter, B11: Member Initialisation Order Bug, shutdown, Application, cleanup, frameTime, init (+29 more)

### Community 4 - "Build System and Dependencies"
Cohesion: 0.08
Nodes (38): scrap_warnings Interface Target, ScrapEngine CMake Project, Shared AudioDevice, B13: MSBuild Only, No CI, B2: Absolute Include Paths, B3: Missing Link Libraries, B5: Game Compiled Into Engine, D7: CMake and vcpkg (+30 more)

### Community 5 - "Graphics Draw Parameters"
Cohesion: 0.06
Nodes (32): Graphics, vec3, RenderParams, rotationZ, scale, texture, tint, translation (+24 more)

### Community 6 - "Texture Allocation"
Cohesion: 0.07
Nodes (31): drawMesh, ScrapGameEngine::SpriteRenderer::render(), AllocatedTexture, refCount, texture, string, string, unordered_map (+23 more)

### Community 7 - "Text and Glyph Rendering"
Cohesion: 0.08
Nodes (32): GameObject, string, Glyph, advance, offset, size, textureCoords, string (+24 more)

### Community 8 - "Transform Hierarchy"
Cohesion: 0.09
Nodes (30): GameObject, vec2, vec2, vector, ScrapGameEngine::Transform::calculateWorldScale(), ScrapGameEngine::Transform::getLocalScale(), ScrapGameEngine::Transform::getParent(), ScrapGameEngine::Transform::getPosition() (+22 more)

### Community 9 - "Tweening and Easing"
Cohesion: 0.09
Nodes (29): EasingType, function, GameObject, TweenType, vec3, getEasingFunction(), EasingType, function (+21 more)

### Community 10 - "GameObject Composition"
Cohesion: 0.12
Nodes (24): addComponent(), GameObject, string, GameObject, components, componentsJustAdded, Create, destroy (+16 more)

### Community 11 - "Load Scene"
Cohesion: 0.15
Nodes (17): string, GameObject, string, unique_ptr, LoadScene, gameObject, load, _loadTime (+9 more)

### Community 12 - "Button Interaction"
Cohesion: 0.10
Nodes (21): Button, boxOpacity, _color, _hoverColor, isMouseOver, _isPressed, onClick, OnCursorEntered (+13 more)

### Community 13 - "Sprite Component State"
Cohesion: 0.11
Nodes (19): string, vec2, vec3, SpriteRenderer, awake, _color, getTexture, _mesh (+11 more)

### Community 14 - "Entity Model and Serialization"
Cohesion: 0.17
Nodes (15): B7: No Serialization, B8: dynamic_cast Component Lookup, D1: EnTT Entity Model, D5: YAML Scene Serialization, BaseComponent, awake, destroy, flaggedForDeletion (+7 more)

### Community 15 - "Camera State"
Cohesion: 0.12
Nodes (16): Camera, aspectRatio, config, getAspectRatio, getMatrix_projection, getMatrix_view, getOrthoSize, isDirty (+8 more)

### Community 16 - "GameObject Registry"
Cohesion: 0.15
Nodes (14): GameObject, string, GameObjectCollection, add, find, gameObjectMap, gameObjects, gameObjectsToAdd (+6 more)

### Community 17 - "Signal Event System"
Cohesion: 0.15
Nodes (7): connect_member(), function, T, Signal, _current_id, _slots, map

### Community 18 - "Hover Sensors"
Cohesion: 0.14
Nodes (13): ScrapGameEngine::Button::render(), vec4, HoverSensor, boxOpacity, halfHeight, halfWidth, height, isHovered (+5 more)

### Community 19 - "Camera Statics and Their Replacement"
Cohesion: 0.18
Nodes (11): B4: Static God Classes, D4: Camera as Component, mat4, vec2, vec3, ScrapGameEngine::Camera::getMatrix_projection(), ScrapGameEngine::Camera::getMatrix_view(), ScrapGameEngine::Camera::getMatrix_viewProjection() (+3 more)

### Community 20 - "Renderer and Editor Prerequisites"
Cohesion: 0.23
Nodes (12): B1: Fixed-Function OpenGL, D2: One RHI, Two Renderers, D6: Dear ImGui Editor, run, getMatrix_viewProjection, init, beginFrame, clear (+4 more)

### Community 21 - "Draw Commands"
Cohesion: 0.17
Nodes (12): DrawCommand, meshId, modelMatrix, rotationZ, scale, textureID, tint, translation (+4 more)

### Community 22 - "Button Implementation"
Cohesion: 0.21
Nodes (9): GameObject, vec2, vec4, ScrapGameEngine::Button::Button(), ScrapGameEngine::Button::setColor(), ScrapGameEngine::Button::setColour(), ScrapGameEngine::Button::setHoverColor(), ScrapGameEngine::Button::setHoverColour() (+1 more)

### Community 23 - "vcpkg Dependency Manifest"
Cohesion: 0.17
Nodes (11): freetype, glfw3, glm, miniaudio, builtin-baseline, dependencies, description, homepage (+3 more)

### Community 24 - "Task Scheduler"
Cohesion: 0.27
Nodes (9): function, function, Scheduler, addDelayedTask, addRepeatingTask, delayedTasks, dispose, repeatingTasks (+1 more)

### Community 25 - "Render Submission"
Cohesion: 0.20
Nodes (8): ScrapGameEngine::Graphics::drawMesh(), mat4, vector, Renderer, draws, isRendering, submitCommand, vpMatrix

### Community 26 - "Public Header Surface"
Cohesion: 0.28
Nodes (3): string, vec4, vec2

### Community 27 - "Camera and Viewport Setup"
Cohesion: 0.25
Nodes (6): recalculate, CameraConfig, orthoSize, vec3, ScrapGameEngine::Camera::init(), setViewport

### Community 30 - "Cross-Platform Targets"
Cohesion: 1.00
Nodes (3): CI Cross-Platform Build Matrix, OpenGL 3.3 Core Baseline, Desktop-First Platform Targets

### Community 31 - "Naming Cleanup and CI Gate"
Cohesion: 0.67
Nodes (3): CI Naming Gate, B12: Coursework Naming, Strangler-Fig Migration

## Knowledge Gaps
- **207 isolated node(s):** `sound`, `filePath`, `looping`, `volume`, `instance` (+202 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **3 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `BaseComponent` connect `Entity Model and Serialization` to `Build System and Dependencies`, `Text and Glyph Rendering`, `Transform Hierarchy`, `Tweening and Easing`, `GameObject Composition`, `Button Interaction`, `Sprite Component State`, `Public Header Surface`, `Engine Header Includes`?**
  _High betweenness centrality (0.244) - this node is a cross-community bridge._
- **Why does `Texture2D` connect `Sprite Rendering` to `Scene Lifecycle`, `Input Handling`, `Graphics Draw Parameters`, `Texture Allocation`, `Sprite Component State`, `Renderer and Editor Prerequisites`, `Public Header Surface`?**
  _High betweenness centrality (0.232) - this node is a cross-community bridge._
- **Why does `AudioSource` connect `Build System and Dependencies` to `Scene Lifecycle`, `Public Header Surface`, `Input Handling`, `Entity Model and Serialization`?**
  _High betweenness centrality (0.098) - this node is a cross-community bridge._
- **What connects `sound`, `filePath`, `looping` to the rest of the system?**
  _207 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Sprite Rendering` be split into smaller, more focused modules?**
  _Cohesion score 0.06826241134751773 - nodes in this community are weakly interconnected._
- **Should `Scene Lifecycle` be split into smaller, more focused modules?**
  _Cohesion score 0.06666666666666667 - nodes in this community are weakly interconnected._
- **Should `Input Handling` be split into smaller, more focused modules?**
  _Cohesion score 0.06763285024154589 - nodes in this community are weakly interconnected._