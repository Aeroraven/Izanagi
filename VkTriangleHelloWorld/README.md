# VkTriangleHelloWorld

Minimal Vulkan triangle sample built with CMake.

## Build
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Run `build/Release/VkTriangleHelloWorld.exe` from the build tree so it can find `triangle.vert.spv` and `triangle.frag.spv`.

## Dependencies
- Vulkan SDK (includes loader, headers, and glslangValidator)
