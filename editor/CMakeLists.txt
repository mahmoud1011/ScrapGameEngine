find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)

add_executable(ScrapEditor
    src/main.cpp
    src/EditorCamera.cpp
    src/EditorContext.cpp
    src/ImGuiLayer.cpp
    src/Panels.cpp
    src/ProjectHub.cpp
    src/ScrapTheme.cpp
)

target_include_directories(ScrapEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(ScrapEditor PRIVATE
    Scrap::Engine
    imgui::imgui
    imguizmo::imguizmo
    scrap_warnings
)

# nethost.dll must sit beside the executable, since we link the import library
# rather than the static one.
if(TARGET DotNetHost::nethost AND DotNetHost_RUNTIME_DLL)
    add_custom_command(TARGET ScrapEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DotNetHost_RUNTIME_DLL}" "$<TARGET_FILE_DIR:ScrapEditor>"
        COMMENT "Staging nethost.dll")
endif()

# The editor writes its dock layout beside the executable, and looks for content
# one level up, where the sandbox assets are staged.
set_target_properties(ScrapEditor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:ScrapEditor>")
