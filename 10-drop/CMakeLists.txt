cmake_minimum_required(VERSION 3.20)
project(DropExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(DropExt SHARED
    dllmain.cpp
    DropHandler.cpp
    DropExt.def
)

target_include_directories(DropExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(DropExt PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(DropExt PRIVATE /utf-8)
