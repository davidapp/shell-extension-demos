cmake_minimum_required(VERSION 3.20)
project(ExplorerCommandExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ExplorerCommandExt SHARED
    dllmain.cpp
    ExplorerCommand.cpp
    ExplorerCommandExt.def
)

# 仓库根目录加入 include 路径，好用 "common/ShellExtBase.h"。
target_include_directories(ExplorerCommandExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_compile_definitions(ExplorerCommandExt PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX
)
target_compile_options(ExplorerCommandExt PRIVATE /utf-8)
target_link_libraries(ExplorerCommandExt PRIVATE shlwapi)
