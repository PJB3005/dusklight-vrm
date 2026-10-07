cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED ENV{CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type")
endif ()

project(dusklight_vrm VERSION 1.0.0 LANGUAGES C CXX)

option(ENABLE_IMGUI "Enable imgui UI for development only")

add_compile_options(
        $<$<COMPILE_LANGUAGE:C,CXX>:/bigobj>
        $<$<COMPILE_LANGUAGE:C,CXX>:/MP>
        $<$<COMPILE_LANGUAGE:C,CXX>:/FS>
        /Zc:__cplusplus # Enable updated `__cplusplus` macro
)

# The Dusklight version to build against: a release tag (e.g. "v2.0.0") or a commit SHA.
if (NOT DUSKLIGHT_VERSION)
    set(DUSKLIGHT_VERSION "v2.0.3")
endif ()

# Fetches the Dusklight sources into ./dusklight and provides DUSKLIGHT_DIR.
# Use -DDUSKLIGHT_DIR= to point at an existing Dusklight checkout.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchDusklight.cmake")
add_subdirectory("${DUSKLIGHT_DIR}/sdk" dusklight-sdk EXCLUDE_FROM_ALL)

configure_file(mod.json.in "${CMAKE_CURRENT_BINARY_DIR}/mod.json" @ONLY NEWLINE_STYLE LF)

set(_features game fmt webgpu)
if (ENABLE_IMGUI)
    list(APPEND _features imgui)
endif ()

add_mod(dusklight_vrm
        FEATURES ${_features}
        SOURCES
            src/config.cpp
            src/debug_imgui.cpp
            src/image_io.cpp
            src/loader.cpp
            src/mod.cpp
            src/render.cpp
            src/scene.cpp
            src/services.cpp
            src/shader.cpp
            src/ui.cpp
        MOD_JSON "${CMAKE_CURRENT_BINARY_DIR}/mod.json"
        RES_DIR res            # mod resources, including icon.png and banner.png
        #OVERLAY_DIR overlay    # game file overlays; remove if unused
        #TEXTURES_DIR textures  # texture replacements; remove if unused
)

include(FetchContent)
include(wesl.cmake)
include(deps.cmake)

FetchContent_Declare(
        glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
        GIT_TAG 	8d1fd52e5ab5590e2c81768ace50c72bae28f2ed #refs/tags/1.0.3
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
        URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
)
FetchContent_MakeAvailable(json)

add_subdirectory(tinygltf)
add_subdirectory(src/helpers)

target_compile_definitions(dusklight_vrm PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_precompile_headers(dusklight_vrm PRIVATE <vector> <memory>)
target_link_libraries(dusklight_vrm PRIVATE tinygltf glm::glm png_static wesl nlohmann_json::nlohmann_json)

if (ENABLE_IMGUI)
    target_compile_definitions(dusklight_vrm PRIVATE ENABLE_IMGUI=1)
endif ()

if (MSVC)
    target_compile_options(dusklight_vrm PRIVATE /guard:cf)
    target_link_options(dusklight_vrm PRIVATE /guard:cf)
    target_link_options(dusklight_vrm PRIVATE /CETCOMPAT)
endif ()
