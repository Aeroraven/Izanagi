# VkHooking

Local Vulkan loader proxy (`vulkan-1.dll`) plus `helper-vulkan.dll` that logs
key creation calls and dumps shader IR while running `VkTriangleHelloWorld`.

## Build (Windows)
```powershell
cmake -S VkHooking -B build/VkHooking -G "Visual Studio 17 2022" -A x64
cmake --build build/VkHooking --config Release
```

## Usage
1. Build `VkTriangleHelloWorld`.
2. Copy `build/VkHooking/Release/vulkan-1.dll` and
   `build/VkHooking/Release/helper-vulkan.dll` next to
   `VkTriangleHelloWorld.exe` (and the `.spv` shader files).
3. Run `VkTriangleHelloWorld.exe`.

Logs and shader dumps land under:
`%USERPROFILE%\Documents\Izanagi_Logs\YYYYMMDD-HHMMSS-vkhook\`
