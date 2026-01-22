# D3D11TriangleHelloWorld

A minimal DirectX 11 sample that renders a colored triangle.

## Build (Windows)
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Run the executable from the build output directory so it can load `shaders.hlsl` beside the binary.

## Dependencies
- Windows 10/11 with a DirectX 11–capable GPU/driver
- Visual Studio 2022 Build Tools (or full VS) with MSVC and Windows 10/11 SDK
- CMake 3.20 or newer