cmake_minimum_required(VERSION 3.20)
project(VkTriangleHelloWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

find_program(GLSLANG_VALIDATOR
    NAMES glslangValidator
    HINTS $ENV{VULKAN_SDK}/Bin $ENV{VULKAN_SDK}/Bin32)

if (NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found. Install the Vulkan SDK and ensure VULKAN_SDK is set.")
endif()

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

set(VERT_SHADER ${SHADER_SOURCE_DIR}/triangle.vert)
set(FRAG_SHADER ${SHADER_SOURCE_DIR}/triangle.frag)
set(VERT_SPV ${SHADER_BINARY_DIR}/triangle.vert.spv)
set(FRAG_SPV ${SHADER_BINARY_DIR}/triangle.frag.spv)

add_custom_command(
    OUTPUT ${VERT_SPV}
    COMMAND ${GLSLANG_VALIDATOR} -V ${VERT_SHADER} -o ${VERT_SPV}
    DEPENDS ${VERT_SHADER}
    COMMENT "Compiling triangle.vert"
)

add_custom_command(
    OUTPUT ${FRAG_SPV}
    COMMAND ${GLSLANG_VALIDATOR} -V ${FRAG_SHADER} -o ${FRAG_SPV}
    DEPENDS ${FRAG_SHADER}
    COMMENT "Compiling triangle.frag"
)

add_custom_target(VulkanShaders DEPENDS ${VERT_SPV} ${FRAG_SPV})

add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    ${VERT_SHADER}
    ${FRAG_SHADER}
)

add_dependencies(${PROJECT_NAME} VulkanShaders)

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

target_compile_definitions(${PROJECT_NAME} PRIVATE _UNICODE UNICODE)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${VERT_SPV}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/triangle.vert.spv
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${FRAG_SPV}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/triangle.frag.spv)
