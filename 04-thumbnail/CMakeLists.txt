cmake_minimum_required(VERSION 3.20)
project(ThumbnailExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ThumbnailExt SHARED
    dllmain.cpp
    ThumbnailProvider.cpp
    ThumbnailExt.def
)

target_include_directories(ThumbnailExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(ThumbnailExt PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(ThumbnailExt PRIVATE /utf-8)
# GDI/user32/ole32/uuid/advapi32 均在 MSVC 默认库里，无需额外链接。
