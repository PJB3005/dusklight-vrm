#
# ZLIB
#

set(ZLIB_BUILD_SHARED OFF CACHE BOOL "Enable building zlib shared library" FORCE)
set(ZLIB_BUILD_TESTING OFF CACHE BOOL "Enable Zlib Examples as tests" FORCE)
set(ZLIB_INSTALL OFF CACHE BOOL "Enable installation of zlib" FORCE)
set(ZLIB_COMPAT ON CACHE BOOL "Use the compat version of zlib instead of the newer zlib-ng" FORCE)
if (MSVC AND CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    # zlib-ng's NEON LD4 probe passes GNU-style -march flags that clang-cl rejects
    set(NEON_HAS_LD4 1 CACHE INTERNAL "")
endif ()
FetchContent_Declare(
        ZLIB
        URL https://github.com/zlib-ng/zlib-ng/archive/refs/tags/2.3.3.tar.gz
        URL_HASH SHA256=f9c65aa9c852eb8255b636fd9f07ce1c406f061ec19a2e7d508b318ca0c907d1
        PATCH_COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/PatchZlib.cmake"
        DOWNLOAD_EXTRACT_TIMESTAMP FALSE
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(ZLIB)
add_library(ZLIB::ZLIB ALIAS zlib-ng-static)
# libpng calls find_package(ZLIB) even when zlib was already provided by FetchContent.
file(WRITE "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/ZLIBConfig.cmake"
        "set(ZLIB_FOUND TRUE)\n"
        "set(ZLIB_INCLUDE_DIR \"${zlib_SOURCE_DIR}\")\n"
        "set(ZLIB_INCLUDE_DIRS \"${zlib_BINARY_DIR};${zlib_SOURCE_DIR}\")\n"
)

#
# LibPNG
#

set(PNG_SHARED OFF CACHE BOOL "Build libpng as a shared library" FORCE)
set(PNG_TESTS OFF CACHE BOOL "Build the libpng tests" FORCE)
set(PNG_TOOLS OFF CACHE BOOL "Build the libpng tools" FORCE)
FetchContent_Declare(
        PNG
        URL https://github.com/pnggroup/libpng/archive/refs/tags/v1.6.58.tar.gz
        URL_HASH SHA256=A9D4DF463D36A6E5F9C29BD6F4967312D17E996C1854F3511F833924EB1993CF
        DOWNLOAD_EXTRACT_TIMESTAMP FALSE
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(PNG)
add_library(PNG::PNG ALIAS png_static)