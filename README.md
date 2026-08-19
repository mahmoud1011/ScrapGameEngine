<a id="readme-top"></a>

# ScrapEngine

A cross-platform 2D/3D game engine written in C++17, with an editor-driven workflow.

ScrapEngine is built around a small engine library that games and tools both link
against. It targets Windows, macOS and Linux today; Android and iOS are planned once
the render backend supports them.

> **Status: under active reconstruction.** The engine is being rebuilt from its 0.7
> coursework origins into an editor-driven 2D/3D engine. See
> [docs/ENGINE_IMPROVEMENT_PLAN.md](docs/ENGINE_IMPROVEMENT_PLAN.md) for the
> architecture, the decisions behind it, and the phase roadmap. The renderer is
> currently still the legacy fixed-function path; Phase 1 replaces it.

## Building

### Requirements

- **CMake** 3.21 or newer
- **A C++17 compiler** — MSVC 2022+, Clang 12+, or GCC 11+
- **Ninja** (or any CMake generator you prefer)
- **vcpkg** — provides GLFW, GLM, FreeType and miniaudio

Visual Studio ships CMake, Ninja and vcpkg; on Windows you can use those instead of
installing anything separately.

### Set up vcpkg

```bash
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh    # bootstrap-vcpkg.bat on Windows
export VCPKG_ROOT=/path/to/vcpkg
```

The build picks up `VCPKG_ROOT` automatically. Dependencies are declared in
`vcpkg.json` and installed into the build tree on first configure — nothing is
vendored into the repository and nothing is installed system-wide.

### Configure and build

```bash
cmake --preset windows && cmake --build --preset windows
```

Substitute `linux` or `macos` for the preset on those platforms. The example game
lands at `build/<preset>/bin/Sandbox`, with the engine library in
`build/<preset>/lib`.

To generate a Visual Studio solution instead of using Ninja:

```bash
cmake -B build/vs -G "Visual Studio 17 2022" -A x64
```

## Layout

```
engine/       ScrapEngine library - core, platform, renderer, scene, audio, ui
sandbox/      Example game, and the proving ground for engine changes
thirdparty/   Vendored code that is not worth fetching (glad, stb)
docs/         Architecture plan and Doxygen configuration
```

Engine headers are included module-qualified — `#include "renderer/Camera.h"` — and
move behind a `<Scrap/...>` public API in Phase 1.

## Third-party

| Library | Role | License |
|---|---|---|
| [GLFW](https://www.glfw.org/) | Windowing and input | zlib/libpng |
| [GLM](https://github.com/g-truc/glm) | Vector and matrix maths | MIT |
| [FreeType](https://freetype.org/) | Font rasterisation | FTL / GPLv2 |
| [miniaudio](https://miniaud.io/) | Audio playback and mixing | MIT-0 / Public domain |
| [glad](https://glad.dav1d.de/) | OpenGL function loading | MIT |
| [stb_image](https://github.com/nothings/stb) | Image decoding | MIT / Public domain |

## License

[MIT](LICENSE)
