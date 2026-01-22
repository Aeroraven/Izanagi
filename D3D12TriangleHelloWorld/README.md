# DirectX12TriangleHelloWorld

Minimal DirectX 12 triangle sample built with CMake.

## Build
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Run `build/Release/DirectX12TriangleHelloWorld.exe` from the build tree so it can find `shaders.hlsl`.
