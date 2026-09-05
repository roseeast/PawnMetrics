cmake_minimum_required(VERSION 3.20)

project(pawn_metrics VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

set(SAMP_PLUGIN_SDK_DIR "" CACHE PATH "Path to samp-plugin-sdk checkout. If empty, CMake fetches it from GitHub.")

if(SAMP_PLUGIN_SDK_DIR)
    set(SAMP_PLUGIN_SDK_INCLUDE_DIR "${SAMP_PLUGIN_SDK_DIR}")
else()
    FetchContent_Declare(
        samp-plugin-sdk
        GIT_REPOSITORY https://github.com/maddinat0r/samp-plugin-sdk.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(samp-plugin-sdk)
    set(SAMP_PLUGIN_SDK_INCLUDE_DIR "${samp-plugin-sdk_SOURCE_DIR}")
endif()

add_library(pawn_metrics SHARED
    src/metrics_store.cpp
    src/metrics_server.cpp
    src/natives.cpp
    src/plugin.cpp
    ${SAMP_PLUGIN_SDK_INCLUDE_DIR}/amxplugin.cpp
)

target_include_directories(pawn_metrics PRIVATE
    src
    ${SAMP_PLUGIN_SDK_INCLUDE_DIR}
    ${SAMP_PLUGIN_SDK_INCLUDE_DIR}/amx
)

if(WIN32)
    target_compile_definitions(pawn_metrics PRIVATE WIN32 HAVE_STDINT_H)
    target_link_libraries(pawn_metrics PRIVATE ws2_32)
    target_sources(pawn_metrics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/exports/pawn_metrics.def)
    if(MINGW)
        target_compile_options(pawn_metrics PRIVATE "-includestddef.h")
        target_link_options(pawn_metrics PRIVATE -static -Wl,--kill-at)
    endif()
else()
    target_compile_definitions(pawn_metrics PRIVATE LINUX HAVE_STDINT_H)
    target_link_libraries(pawn_metrics PRIVATE pthread)
    target_compile_options(pawn_metrics PRIVATE -Wall -Wextra -Wpedantic -fvisibility=hidden)
    target_link_options(pawn_metrics PRIVATE -Wl,--no-undefined)
endif()

set_target_properties(pawn_metrics PROPERTIES
    PREFIX ""
    OUTPUT_NAME "pawn_metrics"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

set(OPENMP_SDK_DIR "" CACHE PATH "Path to open.mp-sdk checkout. If empty, CMake fetches it from GitHub.")
option(PAWN_METRICS_BUILD_OPENMP_COMPONENT "Build native open.mp component" ON)

if(PAWN_METRICS_BUILD_OPENMP_COMPONENT)
    if(EXISTS "${OPENMP_SDK_DIR}/include/sdk.hpp")
        set(OPENMP_SDK_INCLUDE_DIR "${OPENMP_SDK_DIR}/include")
        set(OPENMP_SDK_ROOT "${OPENMP_SDK_DIR}")
    else()
        FetchContent_Declare(
            openmp-sdk
            GIT_REPOSITORY https://github.com/openmultiplayer/open.mp-sdk.git
            GIT_TAG master
            GIT_SUBMODULES_RECURSE TRUE
        )
        FetchContent_MakeAvailable(openmp-sdk)
        set(OPENMP_SDK_INCLUDE_DIR "${openmp-sdk_SOURCE_DIR}/include")
        set(OPENMP_SDK_ROOT "${openmp-sdk_SOURCE_DIR}")
    endif()

    add_library(pawn_metrics_component SHARED
        src/metrics_store.cpp
        src/metrics_server.cpp
        src/natives.cpp
        components/openmp/component.cpp
        ${SAMP_PLUGIN_SDK_INCLUDE_DIR}/amxplugin.cpp
    )

    target_include_directories(pawn_metrics_component PRIVATE
        src
        ${SAMP_PLUGIN_SDK_INCLUDE_DIR}
        ${SAMP_PLUGIN_SDK_INCLUDE_DIR}/amx
        ${OPENMP_SDK_INCLUDE_DIR}
        ${OPENMP_SDK_ROOT}/lib/glm
        ${OPENMP_SDK_ROOT}/lib/robin-hood-hashing/src/include
        ${OPENMP_SDK_ROOT}/lib/span-lite/include
        ${OPENMP_SDK_ROOT}/lib/string-view-lite/include
    )

    if(WIN32)
        target_compile_definitions(pawn_metrics_component PRIVATE WIN32 HAVE_STDINT_H)
        target_include_directories(pawn_metrics_component PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/components/openmp/compat)
        target_link_libraries(pawn_metrics_component PRIVATE ws2_32)
        target_sources(pawn_metrics_component PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/exports/pawn_metrics_component.def)
        if(MINGW)
            target_compile_options(pawn_metrics_component PRIVATE "-includestddef.h")
            target_link_options(pawn_metrics_component PRIVATE -static -Wl,--kill-at)
        endif()
    else()
        target_compile_definitions(pawn_metrics_component PRIVATE LINUX HAVE_STDINT_H)
        target_link_libraries(pawn_metrics_component PRIVATE pthread)
        target_compile_options(pawn_metrics_component PRIVATE -Wall -Wextra -fvisibility=hidden)
        target_link_options(pawn_metrics_component PRIVATE -Wl,--no-undefined)
    endif()

    set_target_properties(pawn_metrics_component PROPERTIES
        PREFIX ""
        OUTPUT_NAME "pawn_metrics"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/components"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/components"
    )
endif()
