cmake_minimum_required(VERSION 3.20)
project(PropSheetExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(PropSheetExt SHARED
    dllmain.cpp
    PropSheetHandler.cpp
    PropSheet.rc          # 对话框资源（属性页模板）
    PropSheetExt.def
)

target_include_directories(PropSheetExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(PropSheetExt PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(PropSheetExt PRIVATE /utf-8)
# comctl32 提供 CreatePropertySheetPage / DestroyPropertySheetPage。
target_link_libraries(PropSheetExt PRIVATE comctl32)
