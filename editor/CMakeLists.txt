find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)

# The .rc carries the executable icon and version info. MSVC and MinGW both
# compile .rc files as part of the target; other platforms get their icon from a
# bundle or a .desktop file, so it is Windows-only by nature rather than by choice.
set(SCRAP_EDITOR_RESOURCES "")
if(WIN32)
    list(APPEND SCRAP_EDITOR_RESOURCES ScrapEditor.rc)
endif()

add_executable(ScrapEditor
    ${SCRAP_EDITOR_RESOURCES}
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

# Branding lives beside the binary so the running window can load its icon without
# knowing where the source tree is.
add_custom_command(TARGET ScrapEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:ScrapEditor>/resources"
    COMMENT "Staging branding resources")

# The editor writes its dock layout beside the executable, and looks for content
# one level up, where the sandbox assets are staged.
set_target_properties(ScrapEditor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:ScrapEditor>")
