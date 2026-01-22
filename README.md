# Izanagi Experiments

A collection of small OS/DirectX/DLL experiments. Each experiment lives in its own folder and builds independently.

## Current experiments
- D3D11TriangleHelloWorld — renders a basic triangle with DirectX 11 (C++/CMake).
- D3D11Hooking — local D3D11 proxy + helper DLL for logging and shader bytecode dumps.
- D3D12TriangleHelloWorld — renders a basic triangle with DirectX 12 (C++/CMake).
- D3D12Hooking — local D3D12 proxy + helper DLL to load RenderDoc for frame capture.

## Prerequisites (Windows)
- Windows 10/11 with a DirectX 11/12–capable GPU/driver
- Visual Studio 2022 Build Tools (or full VS) with MSVC and Windows 10/11 SDK (VS 17 or VS 18 Insider)
- CMake 3.20 or newer

## Build the DirectX 11 sample
```powershell
cmake -S D3D11TriangleHelloWorld -B build/D3D11TriangleHelloWorld -G "Visual Studio 17 2022" -A x64
cmake --build build/D3D11TriangleHelloWorld --config Release
```
Run the executable from the build output directory so it can load `shaders.hlsl` beside the binary.

## Build D3D11Hooking
```powershell
cmake -S D3D11Hooking -B build/D3D11Hooking -G "Visual Studio 17 2022" -A x64
cmake --build build/D3D11Hooking --config Release
```
Copy `build/D3D11Hooking/Release/d3d11.dll` and `build/D3D11Hooking/Release/helper.dll` next to
`D3D11TriangleHelloWorld` to enable the hook.

## Build the DirectX 12 sample
```powershell
cmake -S D3D12TriangleHelloWorld -B build/D3D12TriangleHelloWorld -G "Visual Studio 17 2022" -A x64
cmake --build build/D3D12TriangleHelloWorld --config Release
```
Run the executable from the build output directory so it can load `shaders.hlsl` beside the binary.

See `AGENTS.md` for contribution guidelines.
