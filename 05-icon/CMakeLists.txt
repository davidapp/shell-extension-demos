cmake_minimum_required(VERSION 3.20)
project(IconExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(IconExt SHARED
    dllmain.cpp
    IconHandler.cpp
    IconExt.def
)

target_include_directories(IconExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(IconExt PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(IconExt PRIVATE /utf-8)
