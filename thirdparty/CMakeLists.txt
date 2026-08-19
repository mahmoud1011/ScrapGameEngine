# Vendored third-party code that is not available (or not worth pulling) from vcpkg.
# glad is generated per GL profile, so it is checked in rather than fetched.

add_library(glad STATIC glad/glad.c)
target_include_directories(glad SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/glad")
find_package(OpenGL REQUIRED)
target_link_libraries(glad PUBLIC OpenGL::GL)
if(UNIX AND NOT APPLE)
    target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})
endif()

# stb_image is header-only.
add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")
