# Izanagi Experiments

A collection of small OS/DirectX/DLL experiments. Each experiment lives in its own folder and builds independently.

## Current experiments
- DirectX12TriangleHelloWorld — renders a basic triangle with DirectX 12 (C++/CMake).
- D3D12Hooking — local D3D12 proxy + helper DLL to load RenderDoc for frame capture.

## Prerequisites (Windows)
- Windows 10/11 with a DirectX 12–capable GPU/driver
- Visual Studio 2022 Build Tools (or full VS) with MSVC and Windows 10/11 SDK (VS 17 or VS 18 Insider)
- CMake 3.20 or newer

## Build the DirectX 12 sample
```powershell
cmake -S DirectX12TriangleHelloWorld -B build/DirectX12TriangleHelloWorld -G "Visual Studio 17 2022" -A x64
cmake --build build/DirectX12TriangleHelloWorld --config Release
```
Run the executable from the build output directory so it can load `shaders.hlsl` beside the binary.

See `AGENTS.md` for contribution guidelines.
