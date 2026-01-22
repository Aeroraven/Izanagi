cmake_minimum_required(VERSION 3.20)

project(D3D12Hooking LANGUAGES CXX ASM_MASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(helper SHARED
    src/helper.cpp
)

target_compile_definitions(helper PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

add_library(d3d12 SHARED
    src/d3d12_proxy.cpp
    src/d3d12_stubs.asm
)

target_compile_definitions(d3d12 PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(d3d12 PRIVATE helper)
if (MSVC)
    target_link_options(d3d12 PRIVATE "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/src/d3d12.def")
endif()
