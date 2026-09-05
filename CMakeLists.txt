cmake_minimum_required(VERSION 3.16)

project(pawn_metrics VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PAWN_METRICS_BUILD_SHARED "Build pawn-metrics as a SA-MP/open.mp plugin" ON)

set(SAMP_PLUGIN_SDK_DIR "" CACHE PATH "Path to SA-MP/open.mp plugin SDK headers")

if(NOT SAMP_PLUGIN_SDK_DIR)
    message(FATAL_ERROR "Set SAMP_PLUGIN_SDK_DIR to a folder containing plugin.h and amx/amx.h")
endif()

add_library(pawn_metrics SHARED
    src/metrics_store.cpp
    src/metrics_server.cpp
    src/plugin.cpp
)

target_include_directories(pawn_metrics PRIVATE
    src
    ${SAMP_PLUGIN_SDK_DIR}
    ${SAMP_PLUGIN_SDK_DIR}/amx
)

if(WIN32)
    target_link_libraries(pawn_metrics PRIVATE ws2_32)
    set_target_properties(pawn_metrics PROPERTIES OUTPUT_NAME "pawn_metrics")
else()
    target_link_libraries(pawn_metrics PRIVATE pthread)
    set_target_properties(pawn_metrics PROPERTIES
        OUTPUT_NAME "pawn_metrics"
        PREFIX ""
        SUFFIX ".so"
    )
endif()

