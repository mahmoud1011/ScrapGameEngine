cmake_minimum_required(VERSION 3.21)

# vcpkg toolchain must be picked up before project(). Honour an explicit
# CMAKE_TOOLCHAIN_FILE, otherwise fall back to a VCPKG_ROOT in the environment.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "vcpkg toolchain file")
endif()

project(ScrapEngine
    VERSION 0.8.0
    DESCRIPTION "A cross-platform 2D/3D game engine"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Single output tree, so assets sit next to the executables that load them.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Multi-config generators (VS, Xcode) append a per-config subdir; strip it so
# every configuration lands in the same place on every platform.
foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${cfg} CFG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFG} "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFG} "${CMAKE_BINARY_DIR}/lib")
endforeach()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(SCRAP_BUILD_SANDBOX "Build the sandbox example game" ON)

# Warnings are shared by every first-party target. Deliberately not -Werror yet:
# the 0.7 sources still trip a number of these, and Phase 1 clears them.
add_library(scrap_warnings INTERFACE)
if(MSVC)
    target_compile_options(scrap_warnings INTERFACE /W4 /permissive-)
    # Stop windows.h leaking min/max macros into anything that includes it.
    target_compile_definitions(scrap_warnings INTERFACE NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    target_compile_options(scrap_warnings INTERFACE
        -Wall -Wextra -Wpedantic -Wreorder -Wno-unused-parameter)
endif()

add_subdirectory(thirdparty)
add_subdirectory(engine)

if(SCRAP_BUILD_SANDBOX)
    add_subdirectory(sandbox)
endif()

message(STATUS "ScrapEngine ${PROJECT_VERSION} - ${CMAKE_SYSTEM_NAME} / ${CMAKE_CXX_COMPILER_ID}")
