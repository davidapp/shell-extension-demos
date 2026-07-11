cmake_minimum_required(VERSION 3.20)
project(ContextMenuExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 生成 DLL；.def 文件负责导出 COM 服务器的四个标准入口。
add_library(ContextMenuExt SHARED
    dllmain.cpp
    ClassFactory.cpp
    ContextMenuHandler.cpp
    ContextMenuExt.def
)

target_compile_definitions(ContextMenuExt PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX
)

# 源文件是 UTF-8（无 BOM）。在中文 locale 的机器上，MSVC 默认按 GB2312(cp936) 读源码，
# 会把中文注释/字符串读乱（报 "语法错误" / "字符串字面量中的换行符"）。/utf-8 明确告诉它按 UTF-8 读。
target_compile_options(ContextMenuExt PRIVATE /utf-8)

# shlwapi 提供 StrCmpIA 等；shell32/user32/advapi32/ole32/uuid 已在 MSVC 默认库里。
target_link_libraries(ContextMenuExt PRIVATE shlwapi)
