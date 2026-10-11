option(WESL_AUTOMATIC "Automatically build WESL" ON)

if (WESL_AUTOMATIC)
    FetchContent_Declare(
            Corrosion
            GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
            GIT_TAG 1499b14e4906a2890f5cee1547c8848db261753d
    )
    FetchContent_MakeAvailable(Corrosion)

    FetchContent_Declare(
            wesl
            GIT_REPOSITORY https://github.com/webgpu-tools/wesl-rs.git
            GIT_TAG 3ab716c0910e77f30c20415c9263cfaf6994f777
    )
    FetchContent_MakeAvailable(wesl)

    corrosion_import_crate(MANIFEST_PATH ${wesl_SOURCE_DIR}/Cargo.toml CRATE_TYPES staticlib IMPORTED_CRATES _wawa)
    target_include_directories(wesl_c INTERFACE "${wesl_SOURCE_DIR}/crates/wesl-c/include")
    add_library(wesl ALIAS wesl_c)

else ()
    set(WESL_DIR "" CACHE STRING "Path to wesl-rs")
    set(WESL_BIN_DIR "" CACHE STRING "Path to wesl-rs target directory")

    add_library(wesl STATIC IMPORTED)
    if (WIN32)
        set(_static_lib_name "wesl_c.lib")
    else ()
        set(_static_lib_name "libwesl_c.a")
    endif ()

    if (WESL_BIN_DIR STREQUAL "")
        set(_bin_dir "${WESL_DIR}/target/release")
    else ()
        set(_bin_dir ${WESL_BIN_DIR})
    endif ()

    set_target_properties(wesl PROPERTIES IMPORTED_LOCATION "${_bin_dir}/${_static_lib_name}")
    target_include_directories(wesl INTERFACE "${WESL_DIR}/crates/wesl-c/include")

    if (WIN32)
        target_link_libraries(wesl INTERFACE Ws2_32.lib ntdll.lib Userenv.lib )
    endif ()
endif ()
