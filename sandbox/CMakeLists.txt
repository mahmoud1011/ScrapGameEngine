add_executable(Sandbox
    src/main.cpp
    src/GameScene.cpp
    src/LoadScene.cpp
    src/MainMenuScene.cpp
    src/SplashScreenScene.cpp
)

target_link_libraries(Sandbox PRIVATE Scrap::Engine scrap_warnings)

# The sandbox loads assets as "../assets/...", relative to the working directory.
# Executables land in <build>/bin, so the content root is staged as its sibling.
# Phase 5's asset pipeline replaces this with a resolved content root.
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Sandbox>/../assets"
    COMMENT "Staging sandbox assets")

# Make F5 in Visual Studio work without hand-configuring the debugger.
set_target_properties(Sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Sandbox>")

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT Sandbox)
endif()
