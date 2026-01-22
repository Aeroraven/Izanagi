cmake_minimum_required(VERSION 3.20)
project(DirectX12TriangleHelloWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    shaders.hlsl
)

set_source_files_properties(shaders.hlsl PROPERTIES VS_TOOL_OVERRIDE "None")

target_compile_definitions(${PROJECT_NAME} PRIVATE _UNICODE UNICODE)

target_link_libraries(${PROJECT_NAME} PRIVATE d3d12 dxgi d3dcompiler)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders.hlsl
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders.hlsl)
