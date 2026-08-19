# ScrapEngine — Architecture & Improvement Plan

Branch: `EngineImprovement`
Status: **Phases 0, 1 and 2 complete.** C# scripting hosted. Phase 3 (scene model) next.
Scope: take ScrapGameEngine 0.7 from a 2D coursework framework to **ScrapEngine**, an editor-driven 2D/3D engine.

Platform targets: Windows, macOS and Linux first; Android and iOS once the render
backend supports them. Reference workflow is Unity's — an editor that opens a project,
edits scenes live, and builds standalone players from them.

---

## 1. Where the engine is today

The 0.7 codebase is ~30 translation units in one flat folder, C++17, MSVC v143, built by a single `.vcxproj` that produces one executable containing engine *and* game. It uses GLFW, GLAD, GLM, stb_image, FreeType and irrKlang.

**What genuinely works and should be ported, not rewritten:**

| Subsystem | File | Note |
|---|---|---|
| Event/signal system | `Signal.h` | Clean, templated, no changes needed |
| Deferred task scheduler | `Scheduler.{h,cpp}` | Useful as-is |
| Tweening | `TweenComponent.{h,cpp}` | Port to the new component model |
| Keyboard/mouse input | `Input.{h,cpp}` | Good API, needs de-static-ing |
| Text rendering | `Text.{h,cpp}` | FreeType glyph atlas, port to batched renderer |
| Transform hierarchy | `Transform.{h,cpp}` | Parent/child logic is sound; needs vec3 |
| Scene lifecycle | `BaseScene.h` | Sensible virtual lifecycle |

The *ideas* in this engine are mostly right. The problems are in the foundation layer beneath them.

---

## 2. Blocking issues

Ordered by how hard they block the editor and 3D.

### B1 — The renderer is fixed-function OpenGL

`Renderer.cpp` draws with `glPushMatrix`, `glLoadMatrixf`, `glMultMatrixf`, `glEnableClientState`, `glVertexPointer`, `glColor4fv`. This is the OpenGL 1.x compatibility pipeline.

Consequences:

- No shaders, so **no 3D lighting, no materials, no post-processing**.
- Dear ImGui's GL backend needs a 3.0+ context, so **no editor**.
- One `glDrawArrays` per sprite — no batching.

This single issue blocks both stated goals. Everything else is downstream of it.

### B2 — The project cannot build from a clean clone

`project/src/xbgt3124_t03.vcxproj:114` hardcodes absolute include paths into a directory that is not in this repository:

```
D:\Assigments\Year3\Sem2\Game Engine Architecture\ToGoGameEngine_0.4\ToGoGameEngine_0.4\deps\include\glad; ...
```

Note it points at `ToGoGameEngine_0.4`, a different engine folder entirely.

### B3 — The link libraries are missing from the repo

The project links `freetype.lib; irrKlang.lib; glfw3.lib; opengl32.lib`, but `deps/lib/` contains exactly one file: `irrKlang.exp`. The root `.gitignore` excludes `*.lib`, `*.a` and `*.dll`, so the binaries were never committed. Combined with B2, **nobody can build this repository**, including you on a fresh machine.

### B4 — Seven all-static god classes

`Camera`, `GameObjectCollection`, `Graphics`, `Input`, `Renderer`, `SceneStateMachine` and `TextureAllocator` are all declared as `class X { X() = delete; /* everything static */ }`.

An editor needs two cameras alive at once (editor fly-camera + the game's camera), two viewports, and the ability to tear down and rebuild a world on Play/Stop without restarting the process. Global static state makes all three impossible. This is the main architectural blocker for the editor.

### B5 — Game code is compiled into the engine

`GameScene`, `MainMenuScene`, `SplashScreenScene`, `LoadScene` and `main.cpp` live in the same target as `Renderer` and `Input`. There is no engine library, so there is nothing an editor or a shipped game could link against.

### B6 — Transform is 2D-only

`Transform.h` stores `glm::vec2 localPosition`, `float localRotation`, `glm::vec2 localScale`. `Camera.cpp:31` only ever builds `glm::ortho`. 3D is not expressible.

### B7 — No serialization exists

There is no way to write a scene to disk or read one back. An editor that cannot save is a toy, so this has to land alongside the editor, not after it.

### B8 — Component lookup is `dynamic_cast` in a loop

`GameObject::getComponent<T>()` walks every component doing a `dynamic_cast` per element. That is O(n) with RTTI cost, per lookup, per frame.

### B9 — Dead, non-compiling code in the tree

`ResourceManagementSystem.h` includes `"ResourceAllocator.h"`, which does not exist anywhere in the repo. Neither `ResourceManagementSystem.h` nor `IResourceManager.h` is referenced by the `.vcxproj` — they are orphans. Separately, `IResourceManager::loadResource` is a non-virtual template that unconditionally returns `nullptr`, so that "interface" cannot dispatch to its derived class at all.

### B10 — The frame limiter is a busy-wait

`Time::processTime` spins in a `do { glfwGetTime(); } while (deltaTime < frameTime);` loop. This burns a full CPU core to cap the framerate.

### B11 — Member initialisation order bug

In `Application.h`, `AppWindow window;` is declared at line 75 and `AppWindowData windowData;` at line 76, but the constructor initialiser list runs `windowData(...), window(windowData)`. C++ initialises members in **declaration order**, so `window` is constructed from an uninitialised `windowData`. This is undefined behaviour, currently masked because `init()` reassigns `windowData` before it is used.

### B12 — Coursework naming throughout

See the inventory in section 5.

### B13 — MSBuild-only, no CI

No CMake, so no portability, no CI, no multi-target build.

---

## 3. Target architecture

Layered, four build targets, one shared engine library.

```
                    +---------------------+
                    |    ScrapEditor.exe  |   ImGui panels, gizmos, EditorCamera
                    +----------+----------+
                               |
+------------------+           |          +---------------------+
| ScrapRuntime.exe |-----------+----------|   Sandbox (game)    |
+------------------+           |          +---------------------+
                               v
                    +---------------------+
                    |    ScrapEngine.lib  |
                    +---------------------+
                               |
   +--------+--------+---------+---------+---------+--------+
   |        |        |         |         |         |        |
 core   platform    rhi    renderer    scene    assets   audio
   |        |        |         |         |         |        |
 Log     Window   Shader   Renderer2D  Scene   AssetMgr  Device
 Time    Input    Buffer   Renderer3D  Entity  Importer  Source
 Event   FileSys  Texture  Material    Comps   Handle    Clip
 UUID    Dialogs  FBO      Camera      Serial  Cache
 Layer            Pipeline
```

### Directory layout

```
ScrapEngine/
├─ CMakeLists.txt
├─ vcpkg.json                  # dependency manifest
├─ cmake/                      # helper modules
├─ engine/
│  ├─ include/Scrap/           # PUBLIC API — the engine's "interface"
│  └─ src/
│     ├─ core/                 # Application, LayerStack, Log, Time, UUID, Event
│     ├─ platform/             # Window(GLFW), Input, FileSystem
│     ├─ rhi/                  # abstract RHI + rhi/opengl/ backend
│     ├─ renderer/             # Renderer2D, Renderer3D, Material, Camera
│     ├─ scene/                # Scene, Entity, Components, SceneSerializer
│     ├─ assets/               # AssetManager, handles, importers
│     ├─ audio/                # AudioDevice, AudioSource
│     ├─ physics/              # Phase 5
│     └─ scripting/            # Phase 5
├─ editor/                     # ScrapEditor.exe
│  └─ panels/                  # Viewport, Hierarchy, Inspector, Content, Console
├─ runtime/                    # ScrapRuntime.exe — ships a game, no editor
├─ sandbox/                    # the current GameScene/MainMenuScene demo lands here
├─ thirdparty/
└─ tests/
```

The public/private header split matters: `engine/include/Scrap/` is the contract a game or the editor codes against. Everything under `engine/src/` is free to change. That split *is* the engine interface in the API sense, and it is what makes the editor a normal consumer of the engine rather than a special case.

---

## 4. Key decisions and trade-offs

### D1 — Entity model: EnTT with a thin `Entity` wrapper *(decided)*

`Scene` owns an `entt::registry`. `Entity` is a small value type wrapping `{entt::entity, Scene*}` and exposes the API you already use:

```cpp
Entity e = scene->createEntity("Player");
auto& t  = e.addComponent<TransformComponent>();
auto& sr = e.addComponent<SpriteRendererComponent>(texture);
if (e.hasComponent<CameraComponent>()) { /* ... */ }
```

Why: header-only, MIT, and it gives the three things the editor needs — O(1) typed access, contiguous per-type storage, and **iteration by component type**. `registry.view<TransformComponent, SpriteRendererComponent>()` is simultaneously how the renderer draws, how the serializer walks the scene, and how the inspector enumerates. One mechanism, three consumers.

Trade-off: `GameObject`, `GameObjectCollection` and `BaseComponent` get rewritten, and components become plain data structs with behaviour moving into systems. That is a real cost, but far cheaper now than after an editor is built on the old model. Scheduling it in Phase 3 rather than Phase 1 keeps the tree green while the renderer work happens.

*Rejected:* keeping OOP with a type-id map — cheaper now, but every inspector field would need hand-written reflection and iterate-by-type stays O(n). *Rejected:* hand-rolling an ECS — best learning value, but weeks of work and the subtlest bugs, before any UI exists.

### D2 — Renderer: one RHI, two high-level renderers

`rhi/` is a thin abstraction over Buffer / Texture / Shader / Framebuffer / Pipeline / RenderCommand, with an OpenGL backend. Above it sit `Renderer2D` (batched quads: one dynamic VBO, texture-slot array, one draw call per batch) and `Renderer3D` (forward, PBR-lite).

Target **OpenGL 3.3 core**.

> *Revised.* This decision originally said 4.5 core, for Direct State Access. That does
> not survive the cross-platform requirement: Apple deprecated OpenGL in 2018 and caps
> macOS at 4.1, so DSA (4.5) and compute shaders (4.3) are simply unavailable there.
> 3.3 core is the highest version all three desktop platforms share, and it maps
> cleanly onto GLES 3.0 for Android later. Cost: the RHI loses DSA and gets somewhat
> more verbose in its bind-then-modify calls.

iOS never gets an OpenGL path — it needs Metal. That is precisely the seam the RHI
exists to provide, and the same seam Vulkan would use on Android and desktop.

Both renderers draw into a `Framebuffer` rather than the default backbuffer. That one decision is what lets the editor show the game inside an ImGui panel, and it costs the runtime nothing.

Trade-off: an RHI layer is indirection you do not strictly need while there is only one backend. It earns its place here because it is the seam where Vulkan would later go, and because it forces render state out of the global `Renderer` statics.

### D3 — Transform: one 3D transform, 2D as a special case *(decided)*

`TransformComponent { glm::vec3 translation; glm::vec3 rotation; glm::vec3 scale; }` with `glm::vec2` convenience overloads for 2D code.

Maintaining two transform types would bifurcate the renderer, the physics integration, the serializer and the editor gizmos. A 2D game paying 48 bytes per transform instead of 20 is not a real cost; two parallel hierarchies is.

### D4 — Camera: components, not a static

`CameraComponent { ProjectionType type; float orthoSize, fov, nearClip, farClip; }` plus a separate `EditorCamera` owned by the editor. The editor's camera must fly around a scene while the game's camera sits untouched — precisely what the current `static Camera` cannot express.

### D5 — Serialization: YAML via `yaml-cpp`

Scenes as `.scrapscene`, prefabs as `.scrapprefab`, both YAML.

Chosen over JSON because scene files get committed and merged, and YAML diffs readably where JSON's brace-and-quote noise does not. Binary serialization is a Phase 6 concern for shipped builds only.

Trade-off: `yaml-cpp` is a compiled dependency where `nlohmann::json` is a single header. vcpkg makes that difference small.

### D6 — Editor UI: Dear ImGui (docking branch) + ImGuizmo

The standard for engine tooling. Immediate mode means no state-sync bugs between UI and scene — the panel re-reads the entity every frame, so it is never stale. Docking gives the Unity-style layout; ImGuizmo gives translate/rotate/scale handles. Qt is the alternative and is not worth its weight or its licensing here.

### D7 — Build: CMake + vcpkg manifest

CMake still generates your VS solution (`cmake -G "Visual Studio 17 2022"`), so your workflow barely changes, but the absolute-path problem (B2) and the single-target problem (B5) both disappear. A `vcpkg.json` manifest fetches glfw3, freetype, yaml-cpp, entt, imgui and box2d — fixing the missing-binaries problem (B3) permanently by not vendoring binaries at all.

### D8 — Audio: replace irrKlang with miniaudio

irrKlang is closed-source, ships only prebuilt binaries (currently missing from the repo), and is paid for commercial use. `miniaudio` is a single public-domain header with 3D spatial audio built in — which you need for 3D anyway. This also removes one of the two missing-binary dependencies.

### D9 — Physics: deferred to Phase 5

Box2D for 2D, Jolt for 3D, both behind a `PhysicsWorld` interface. Deliberately kept off the critical path to the editor.

### D10 — Scripting: C# on CoreCLR, alongside C++  *(revised)*

> *Originally: native C++ now, Lua later, with C# via Mono judged too large. Revised
> because C# is a stated requirement, and because CoreCLR makes it materially cheaper
> than the Mono estimate that recommendation was based on.*

Game code can be written in **C++** (compiled into the game target) or **C#** (a managed
assembly loaded at runtime). Both are first-class.

The managed layer hosts **CoreCLR in-process** through the official chain: `nethost`
locates `hostfxr`, `hostfxr` starts a runtime from a `runtimeconfig.json`, and
`load_assembly_and_get_function_pointer` reaches managed entry points. That is the path
.NET documents, and the direction Unity is moving with its own CoreCLR migration.

**Why CoreCLR and not NativeAOT.** AOT produces a faster startup and a smaller
footprint, but it compiles ahead of time and cannot load or unload assemblies at
runtime. Script hot reload is exactly that operation — a collectible
`AssemblyLoadContext` per user assembly, unloaded and rebuilt on change. Giving that up
would defeat the purpose of scripting.

**Interop shape.** Calls cross as raw function pointers over blittable arguments;
managed entry points are `[UnmanagedCallersOnly]`. No reflection on the hot path, and no
managed object is ever held by native code — an entity crosses as an integer handle.
This is the discipline that keeps a scripting layer from becoming the profile's
hot spot.

Trade-off: the boundary is unforgiving. A function pointer signature that disagrees with
its managed declaration corrupts the stack with no diagnostic, so signatures are
declared next to each other by convention. A generated binding layer replaces that
convention later.

*Still deferred:* Lua via sol2, if a lighter embedded option is ever wanted alongside.

## 5. Naming cleanup inventory

Every coursework-derived identifier, and what it becomes:

| Current | Action |
|---|---|
| `ScrapGameEngine_0.7/` | → repo root layout; version lives in git tags, not folder names |
| `0130645_Doxyfile` | → `docs/Doxyfile`; also fix `INPUT`, which points at a nonexistent absolute path |
| `project/src/xbgt3124_t03.vcxproj` | delete — replaced by CMake |
| `project/src/xbgt3124_t03.vcxproj.filters` | delete |
| `project/src/xbgt3124_t03.vcxproj.user` | delete |
| `project/src/~AutoRecover.xbgt3124_t03.vcxproj` | delete — VS autorecover junk |
| `project/src/x64/` (incl. `xbgt3124_t03.log`) | delete + gitignore build output |
| `project/ScrapGameEngine.sln` | delete — it references project `xbgt3124_t03`; CMake regenerates |
| `.documentation/html/index.html` | contains `0130645`; regenerate docs from the new Doxyfile |
| namespace `ScrapGameEngine` | → `Scrap` (product name stays **ScrapEngine**) |
| `MeshAllocater` | → `MeshAllocator` (spelling) |

Grep gate for CI, so this cannot regress:

```bash
git grep -iE "0130645|xbgt3124|ToGoGameEngine" && echo FAIL || echo clean
```

---

## 6. Roadmap

Migration strategy is **strangler-fig, in place**: files move with `git mv` so history follows them, CMake builds the existing code first, and subsystems get modernised one at a time. The tree stays buildable at every commit.

### Phase 0 — Foundation. No features. ✅ **Complete**

> *Goal: it builds from a clean clone, and nothing is named after a student ID.*

- [x] Restructure into `engine/ sandbox/ thirdparty/` via `git mv` (history preserved)
- [x] CMake + `vcpkg.json` + `CMakePresets.json`; deleted `.vcxproj` / `.sln`
- [x] Purged every name in section 5; CI grep gate added
- [x] Moved `GameScene` / `MainMenuScene` / `SplashScreenScene` / `LoadScene` / `main.cpp` → `sandbox/`
- [x] Deleted dead `ResourceManagementSystem.h` + `IResourceManager.h` (B9)
- [x] Fixed B11 (member order) and B10 (busy-wait → sleep-then-spin)
- [x] GitHub Actions building on Windows, macOS and Linux

**Exit criteria — met.** Configures and builds clean on Windows (MSVC 19.51, Ninja);
`ScrapEngine.lib` and `Sandbox.exe` are separate artifacts, so B5 is closed. Sandbox
runs, loads its textures and transitions scenes with no errors on stderr. The naming
grep returns nothing outside `docs/`.

Three things came up that were not in the original plan:

- **Audio had to move in Phase 0, not Phase 5.** irrKlang's libraries were never in
  the repo, so nothing could link on any platform. D8 was pulled forward: there is now
  a shared `AudioDevice` plus a miniaudio-backed `AudioSource`. This also fixes a bug
  where every `AudioSource` component constructed an entire sound engine of its own.
- **`Time.h` was shadowing the C runtime's `<time.h>`.** With `engine/src/core` on the
  include path, Windows' case-insensitive filesystem resolved `<ctime>`'s internal
  `#include <time.h>` to our header, breaking every translation unit that touched
  `<ctime>`. Engine includes are now module-qualified (`"core/Time.h"`) and only
  `engine/src` is on the include path — an early down payment on the Phase 1
  `<Scrap/...>` migration.
- **`Application.cpp` was including `MainMenuScene.h`** — the engine directly including
  a game scene. Removed as part of the engine/game split.

`editor/` and `runtime/` are not scaffolded yet; they arrive in Phases 2 and 6 with
actual content rather than as empty targets.

### Phase 1 — Modern GL core + RHI ✅ **Complete**

> *Goal: identical output, but through shaders. Prerequisite for everything else.*

- [x] glad regenerated for **GL 3.3 core**; the vendored loader only reached 2.1 on the compatibility profile
- [x] GLFW core-profile 3.3 context, forward-compat on macOS
- [x] `rhi/`: Shader, VertexBuffer, IndexBuffer, VertexArray, Framebuffer
- [x] `Renderer2D` batched; `SpriteRenderer` and the HoverSensor debug boxes ported onto it
- [x] Camera is ortho **and** perspective capable; depth buffer enabled

**Exit criteria — mostly met.** Verified on an RTX 3070 Ti: a 3.3.0 core context comes
up, the sandbox renders its load scene correctly through the shader path with nothing on
stderr, and the frame lands in a `Framebuffer` before being blitted to the window. Every
draw goes through a shader — the fixed-function entry points no longer exist in the
loader, so any survivor would be a compile error.

**Not verified: the 10k-sprites-at-60fps target.** No benchmark scene exists yet, and the
sandbox draws only a handful of quads. The batching is implemented (one dynamic VBO, a
16-slot texture array, one draw call per batch) but its throughput is unmeasured. A
stress scene belongs in Phase 2 alongside the editor's stats panel, which is where the
draw-call and quad counters already plumbed through `Renderer::getDrawCallCount` will
surface.

Notes on what Phase 1 turned up:

- **`Text::render()` was empty.** Text rendering was never implemented in 0.7 — the
  FreeType glyph atlas loads but nothing draws it. Porting it is now Phase 2 work, not
  a regression.
- **`Mesh` and `MeshAllocator` are effectively vestigial.** Every sprite was a unit quad
  and the batcher generates that geometry itself, so `Graphics::drawMesh` ignores its
  mesh argument. They stay until Renderer3D gives real meshes a purpose.
- **Three portability bugs surfaced** the moment CI compiled with clang and gcc:
  `<glfw/glfw3.h>` (the header is `GLFW/`), backslash include separators in
  `Graphics.h`, and `switch` cases declaring locals without a scope. All three were
  pre-existing and invisible under MSVC alone.

### Phase 2 — The Editor ✅ **Complete**

> *Goal: a real editor window you can select and move things in — your "interface first".*

- [x] ImGui docking + ImGuizmo, wrapped in an `ImGuiLayer`
- [x] Panels: Viewport (sampling the Phase-1 framebuffer), Hierarchy, Inspector, Content Browser, Console, Stats
- [x] `EditorCamera` — orbit/pan/zoom, fully independent of the engine's Camera
- [x] Default dock layout built programmatically on first run, then the user's own layout persists
- [x] A distinct visual identity: slate chrome, a patina accent used only for state, square corners
- [ ] Mouse picking via an entity-ID attachment readback — **not done**, selection is hierarchy-only for now
- [ ] `Text` rendering — still unimplemented, carried forward

**Exit criteria — met for selection and editing.** The editor opens on a docked
Unity-shaped layout, lists the scene, edits the selected object's transform in the
inspector, and drags it with a gizmo. Viewport picking is the one part of the original
criterion still outstanding.

**It also settled the Phase 1 question.** The Stats panel reads draw calls and quads
straight off the batcher: the starter scene reports **5 quads in 1 draw call**. The
batching works; a scale benchmark is still worth writing, but it is no longer unverified.

Three latent bugs surfaced building it, all pre-existing:

- **`SpriteRenderer` left `_mesh` and `_texture` uninitialised.** Nothing had crashed
  only because the sandbox always called `setTexture()` before rendering. A sprite
  without a texture dereferenced a garbage pointer. `RenderParams::texture` and
  `GameObject::transform` had the same shape and are now defaulted too.
- **`GameObject::Create` never registered with `GameObjectCollection`** — the collection
  was dead code no caller ever populated.
- **`GameObjectCollection::add` double-stored.** It pushed into `gameObjects` while
  `update()` separately folded the same queue in, so every object would have been
  updated and rendered twice. Invisible until something actually used the collection.

Two engine changes the editor forced, both anticipated by the plan:

- `Renderer` gained `beginFrameWith`/`endFrameOffscreen`, so a host can drive the scene
  pass with its own view-projection and skip the window blit. This is D4's static-Camera
  problem showing up concretely.
- `Input::init`/`process` were private and friended to `Application`, which assumed the
  game loop was the only host. They are part of the contract now.

### Phase 3 — Scene model + serialization ← **next**

> *Goal: the editor can save your work.*

- EnTT-backed `Scene`/`Entity`; components: ID(UUID), Tag, Transform, SpriteRenderer, MeshRenderer, Camera, Light, NativeScript
- `SceneSerializer` → `.scrapscene` YAML
- Play / Pause / Stop with copy-on-play, so Stop restores the edit state
- Prefabs
- Bind the hosted C# layer to entities: a `ScriptComponent` naming a managed type, and
  entities crossing the boundary as integer handles. The runtime is already hosted
  (D10); what it lacks is a scene to talk about.

**Exit criteria:** build a scene in the editor, save, close, reopen, hit Play, get your game. Hit Stop, get your edits back.

### Phase 4 — 3D

- `Renderer3D` forward renderer, PBR-lite (albedo / normal / metallic / roughness)
- Directional + point lights; shadow maps
- glTF 2.0 import via `cgltf` / `tinygltf` (Assimp only if FBX/OBJ breadth is needed)
- Cubemap skybox; editor grid + 3D gizmo modes

**Exit criteria:** import a glTF model, light it, move it with gizmos, save and reload the scene.

### Phase 5 — Systems

Physics (Box2D 2D / Jolt 3D) · asset pipeline with UUIDs, async loading and hot reload ·
**C# hot reload** via a collectible `AssemblyLoadContext` per user assembly · viewport
entity picking · `Text` rendering · profiler and instrumentation.

Audio already moved to miniaudio in Phase 0, and the CoreCLR host landed alongside
Phase 2 — what remains here is the reload machinery on top of it.

### Phase 6 — Ship

`ScrapRuntime.exe` · project packaging · asset cooking · binary scene format · documentation.

---

## 7. What to revisit as this grows

- **RHI vs. Vulkan.** The RHI is shaped for GL. The first time you seriously want Vulkan, expect the command-buffer and descriptor/binding model to need rework — the abstraction as designed hides GL's global-state model, not Vulkan's explicit one. Budget for that rather than assuming the seam is free.
- **EnTT in the public API.** If `entt::` types leak into `engine/include/Scrap/`, swapping the ECS later becomes impossible. Keep `Entity` opaque.
- **Forward rendering.** Fine up to a few dozen lights. Past that, deferred or forward+ is the next step, and it changes the material system.
- **Editor and runtime sharing `Scene`.** Right now that is correct and simple. If editor-only data (selection, gizmo state, panel folding) starts creeping into `Scene`, split it into an `EditorContext` before it contaminates the runtime.
- **Single-threaded everything.** There is no job system in this plan. When asset loading or physics starts costing frame time, that is the moment to add one — not before.
