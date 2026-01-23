cmake_minimum_required(VERSION 3.20)

project(VkHooking LANGUAGES CXX ASM_MASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

add_library(helper_vulkan SHARED
    src/helper_vulkan.cpp
)

target_compile_definitions(helper_vulkan PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_include_directories(helper_vulkan PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(helper_vulkan PRIVATE dbghelp)
set_target_properties(helper_vulkan PROPERTIES OUTPUT_NAME "helper-vulkan")

add_library(vulkan_proxy SHARED
    src/vulkan_proxy.cpp
    src/vulkan_stubs.asm
)

target_compile_definitions(vulkan_proxy PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_include_directories(vulkan_proxy PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vulkan_proxy PRIVATE helper_vulkan)
set_target_properties(vulkan_proxy PROPERTIES OUTPUT_NAME "vulkan-1")
if (MSVC)
    target_link_options(vulkan_proxy PRIVATE "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan.def")
endif()
