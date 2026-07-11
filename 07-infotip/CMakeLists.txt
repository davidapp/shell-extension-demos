cmake_minimum_required(VERSION 3.20)
project(InfotipExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(InfotipExt SHARED
    dllmain.cpp
    QueryInfoHandler.cpp
    InfotipExt.def
)

target_include_directories(InfotipExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(InfotipExt PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(InfotipExt PRIVATE /utf-8)
target_link_libraries(InfotipExt PRIVATE shlwapi)
