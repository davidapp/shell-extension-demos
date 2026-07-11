cmake_minimum_required(VERSION 3.20)
project(OverlayExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(OverlayExt SHARED
    dllmain.cpp
    OverlayHandler.cpp
    OverlayExt.def
)

target_include_directories(OverlayExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(OverlayExt PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(OverlayExt PRIVATE /utf-8)
target_link_libraries(OverlayExt PRIVATE Shlwapi)
