# D3D11Hooking

Local `d3d11.dll` proxy + `helper.dll` that logs device/swapchain creation and dumps shader bytecode when running the D3D11 sample.

## Build
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

## Usage
1. Build `D3D11TriangleHelloWorld`.
2. Copy `D3D11Hooking/build/Release/d3d11.dll` and `D3D11Hooking/build/Release/helper.dll` into
   `D3D11TriangleHelloWorld/build/Release` (next to the executable).
3. Launch `DirectX11TriangleHelloWorld.exe` from that folder.
4. Logs and shader dumps appear under:
   `Documents\\Izannagi\\{date}-{time}-d3d11hook\\log\\d3d11.log` and
   `Documents\\Izannagi\\{date}-{time}-d3d11hook\\{id}.vs/.ps/.cs`.

## How it works
- `d3d11.dll` forwards exports to the system `d3d11.dll`.
- `helper.dll` creates the session folder, logs device/swapchain creation, and wraps the device
  to intercept `Create*Shader` calls.
