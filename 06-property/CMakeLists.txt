cmake_minimum_required(VERSION 3.20)
project(PropertyExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(PropertyExt SHARED
    dllmain.cpp
    PropertyStore.cpp
    PropertyExt.def
)

target_include_directories(PropertyExt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(PropertyExt PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(PropertyExt PRIVATE /utf-8)
# propsys 提供 InitPropVariantFromString / PropVariantInit 等。
target_link_libraries(PropertyExt PRIVATE propsys)
