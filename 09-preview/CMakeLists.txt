cmake_minimum_required(VERSION 3.20)
project(PreviewExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(PreviewExt SHARED
    dllmain.cpp
    PreviewHandler.cpp
    PreviewExt.def
)

target_include_directories(PreviewExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(PreviewExt PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(PreviewExt PRIVATE /utf-8)
