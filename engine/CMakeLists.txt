find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_path(MINIAUDIO_INCLUDE_DIR miniaudio.h REQUIRED)

add_library(ScrapEngine STATIC
    src/core/Application.cpp
    src/core/Scheduler.cpp
    src/core/Time.cpp

    src/platform/AppWindow.cpp
    src/platform/Input.cpp

    src/renderer/Camera.cpp
    src/renderer/Graphics.cpp
    src/renderer/Mesh.cpp
    src/renderer/MeshAllocator.cpp
    src/renderer/Renderer.cpp
    src/renderer/Renderer2D.cpp
    src/renderer/SpriteRenderer.cpp
    src/renderer/Text.cpp
    src/renderer/Texture2D.cpp
    src/renderer/TextureAllocator.cpp

    src/scene/BaseComponent.cpp
    src/scene/BaseScene.cpp
    src/scene/GameObject.cpp
    src/scene/GameObjectCollection.cpp
    src/scene/SceneStateMachine.cpp
    src/scene/Transform.cpp
    src/scene/TweenComponent.cpp

    src/rhi/Buffer.cpp
    src/rhi/Framebuffer.cpp
    src/rhi/Shader.cpp
    src/rhi/VertexArray.cpp

    src/ui/Button.cpp

    src/audio/AudioDevice.cpp
    src/audio/AudioSource.cpp
)
add_library(Scrap::Engine ALIAS ScrapEngine)

# Only the src root goes on the include path, never the individual module directories.
# Engine headers are therefore included module-qualified ("renderer/Camera.h"), which
# is both a step toward the Phase 1 <Scrap/...> layout and a correctness requirement:
# with src/core on the path, our Time.h shadows the C runtime's <time.h> on Windows,
# where the filesystem is case-insensitive, and every <ctime> include breaks.
#
# engine/include is the intended public API surface and is populated in Phase 1.
target_include_directories(ScrapEngine
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_include_directories(ScrapEngine SYSTEM PRIVATE "${MINIAUDIO_INCLUDE_DIR}")

target_link_libraries(ScrapEngine
    PUBLIC
        glfw
        glm::glm
        glad
    PRIVATE
        freetype
        stb
        scrap_warnings
)

# miniaudio's backends need the platform audio stack plus threads.
find_package(Threads REQUIRED)
target_link_libraries(ScrapEngine PRIVATE Threads::Threads)
if(APPLE)
    target_link_libraries(ScrapEngine PRIVATE
        "-framework CoreFoundation" "-framework CoreAudio" "-framework AudioToolbox")
elseif(UNIX)
    target_link_libraries(ScrapEngine PRIVATE ${CMAKE_DL_LIBS} m)
endif()
