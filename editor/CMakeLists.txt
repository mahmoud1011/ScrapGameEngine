find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)

add_executable(ScrapEditor
    src/main.cpp
    src/EditorCamera.cpp
    src/EditorContext.cpp
    src/ImGuiLayer.cpp
    src/Panels.cpp
    src/ScrapTheme.cpp
)

target_include_directories(ScrapEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(ScrapEditor PRIVATE
    Scrap::Engine
    imgui::imgui
    imguizmo::imguizmo
    scrap_warnings
)

# The editor writes its dock layout beside the executable, and looks for content
# one level up, where the sandbox assets are staged.
set_target_properties(ScrapEditor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:ScrapEditor>")
