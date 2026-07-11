cmake_minimum_required(VERSION 3.20)
project(W11Menu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 复用第 2 章的 IExplorerCommand 处理器（ExplorerCommand.cpp），只额外写一个精简的 dllmain。
add_library(w11menu SHARED
    dllmain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../02-explorer-command/ExplorerCommand.cpp
    w11menu.def
)

target_include_directories(w11menu PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..                       # common/ShellExtBase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../02-explorer-command   # Guid.h, ExplorerCommand.h
)

target_compile_definitions(w11menu PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(w11menu PRIVATE /utf-8)
target_link_libraries(w11menu PRIVATE shlwapi)

# 把 app.manifest（含 msix 包标识）嵌入 DLL —— 打包后的右键扩展必须有这个嵌入清单，
# 否则资源管理器不会把它当作已打包组件加载。
set_target_properties(w11menu PROPERTIES
    LINK_FLAGS "/MANIFEST:EMBED /MANIFESTINPUT:\"${CMAKE_CURRENT_SOURCE_DIR}/app.manifest\"")
