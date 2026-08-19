# Locates the .NET hosting API (nethost + the hostfxr/coreclr headers).
#
# These ship in the Microsoft.NETCore.App.Host.<rid> pack alongside the SDK rather
# than anywhere a normal find_package would look, so the pack directory is searched
# directly and the newest version wins. The header set is stable across versions and
# nethost resolves whichever runtime is installed at run time, so building against a
# 9.x pack and running on 10.x is fine.
#
# Defines, on success:
#   DotNetHost_FOUND
#   DotNetHost::nethost   imported target carrying the include dir and link library

if(TARGET DotNetHost::nethost)
    set(DotNetHost_FOUND TRUE)
    return()
endif()

if(WIN32)
    set(_dotnet_rid "win-x64")
    set(_dotnet_roots "$ENV{ProgramFiles}/dotnet" "C:/Program Files/dotnet")
elseif(APPLE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        set(_dotnet_rid "osx-arm64")
    else()
        set(_dotnet_rid "osx-x64")
    endif()
    set(_dotnet_roots "/usr/local/share/dotnet" "/opt/homebrew/share/dotnet" "$ENV{HOME}/.dotnet")
else()
    set(_dotnet_rid "linux-x64")
    set(_dotnet_roots "/usr/share/dotnet" "/usr/lib/dotnet" "$ENV{HOME}/.dotnet")
endif()

set(_pack_candidates "")
foreach(_root IN LISTS _dotnet_roots)
    file(GLOB _versions "${_root}/packs/Microsoft.NETCore.App.Host.${_dotnet_rid}/*")
    list(APPEND _pack_candidates ${_versions})
endforeach()

# Newest version first.
list(SORT _pack_candidates COMPARE NATURAL ORDER DESCENDING)

set(_native_dir "")
foreach(_pack IN LISTS _pack_candidates)
    if(EXISTS "${_pack}/runtimes/${_dotnet_rid}/native/nethost.h")
        set(_native_dir "${_pack}/runtimes/${_dotnet_rid}/native")
        break()
    endif()
endforeach()

if(NOT _native_dir)
    set(DotNetHost_FOUND FALSE)
    return()
endif()

# On Windows prefer the import library over the static one. libnethost.lib is built
# against the static release CRT (/MT), so linking it into a /MDd debug build fails
# with _ITERATOR_DEBUG_LEVEL and RuntimeLibrary mismatches. The import library carries
# no CRT of its own; the cost is shipping nethost.dll beside the executable.
if(WIN32)
    # find_library skips the search when its cache entry is already set, so a cache
    # written before this rule preferred the import library would keep pointing at
    # libnethost.lib. Drop such an entry rather than requiring a fresh configure.
    if(DotNetHost_LIBRARY AND DotNetHost_LIBRARY MATCHES "libnethost")
        unset(DotNetHost_LIBRARY CACHE)
    endif()

    find_library(DotNetHost_LIBRARY
        NAMES nethost
        PATHS "${_native_dir}"
        NO_DEFAULT_PATH)
    find_file(DotNetHost_RUNTIME_DLL
        NAMES nethost.dll
        PATHS "${_native_dir}"
        NO_DEFAULT_PATH)
else()
    find_library(DotNetHost_LIBRARY
        NAMES nethost libnethost
        PATHS "${_native_dir}"
        NO_DEFAULT_PATH)
endif()

if(NOT DotNetHost_LIBRARY)
    set(DotNetHost_FOUND FALSE)
    return()
endif()

set(DotNetHost_INCLUDE_DIR "${_native_dir}")
set(DotNetHost_FOUND TRUE)

# GLOBAL so sibling directories see it - find_package runs in engine/, but editor/
# needs the target and the DLL path to stage nethost.dll beside its executable.
add_library(DotNetHost::nethost UNKNOWN IMPORTED GLOBAL)
set_target_properties(DotNetHost::nethost PROPERTIES
    IMPORTED_LOCATION "${DotNetHost_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${DotNetHost_INCLUDE_DIR}")

# nethost.h declares its API dllimport by default, which is right for the import
# library. Only the static variant needs this turned off.
if(DotNetHost_LIBRARY MATCHES "libnethost")
    set_property(TARGET DotNetHost::nethost APPEND PROPERTY
        INTERFACE_COMPILE_DEFINITIONS "NETHOST_USE_AS_STATIC")
endif()

# libnethost is static and pulls in the C++ runtime bits it was built against.
if(WIN32)
    set_property(TARGET DotNetHost::nethost APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES "advapi32" "ole32" "oleaut32")
endif()

message(STATUS "Found .NET hosting API: ${_native_dir}")
