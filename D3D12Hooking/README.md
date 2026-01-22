# D3D12Hooking

Place a custom `d3d12.dll` + `helper.dll` next to the app to allow RenderDoc to hook and capture locally (no system changes).

## Rationale (helper.dll pattern)
Some projects use a `d3d11.dll` proxy plus a small `helper.dll`. That works well for D3D11 because the app typically calls
`D3D11CreateDevice(AndSwapChain)` first, and DXGI objects are created *inside* that call. A proxy can initialize RenderDoc
before the DXGI swapchain exists, without needing a `dxgi.dll` proxy.

For D3D12, the sample creates the DXGI factory *before* `D3D12CreateDevice`, so a late RenderDoc injection can leave you with
an **unwrapped DXGI factory** and a **wrapped D3D12 queue**, which breaks `CreateSwapChainForHwnd`. This project intentionally
avoids a `dxgi.dll`/`dxcore.dll` proxy (to keep things local and minimal), so the reliable way to capture is to ensure RenderDoc
is injected *before* DXGI factory creation.

## Build
```powershell
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Release
```

## Usage
1. Build `D3D12TriangleHelloWorld`.
2. Copy `D3D12Hooking/build/Release/d3d12.dll` and `D3D12Hooking/build/Release/helper.dll` into
   `D3D12TriangleHelloWorld/build/Release`
   (the same folder as the executable).
3. Ensure `renderdoc.dll` is available in `PATH`.
4. Preferred: launch the app **from RenderDoc UI** (or `renderdoccmd`) so injection happens before any DXGI factory is created.
   - If you inject into a **running** process *after* `CreateDXGIFactory*`, you may hit `CreateSwapChainForHwnd` failures.
5. Inject RenderDoc into the process (RenderDoc UI) if you must. The helper does **not** call `LoadLibrary` itself.
6. (Optional) Enable helper logic with `set D3D12_HOOK_RENDERDOC=1`.
7. (Optional) Enable calling `RENDERDOC_GetAPI` with `set D3D12_HOOK_GETAPI=1`.
   - Delay (default 1500ms): `set D3D12_HOOK_GETAPI_DELAY_MS=2000`
   - Wait for device (default on): `set D3D12_HOOK_GETAPI_WAIT_DEVICE=0` to disable
8. (Optional) Enable the F12 capture hotkey with `set D3D12_HOOK_KEYS=1`.
9. (Optional) Enable active-window tracking with `set D3D12_HOOK_ACTIVE_WINDOW=1`.
10. (Optional) Force overlay bits after a window/device is detected with `set D3D12_HOOK_OVERLAY=1` (requires active window).
11. (Optional) Auto-trigger a capture with `set D3D12_HOOK_CAPTURE=1`.
12. (Optional) If using “Inject into Process”, delay device creation so RenderDoc can hook:
   `set D3D12_HOOK_WAIT_RENDERDOC=1` (default wait 5000ms, override with `set D3D12_HOOK_WAIT_MS=8000`).
   - Note: this only delays **before `D3D12CreateDevice`**. If the DXGI factory is already created, this does not fix the swapchain mismatch.
13. Launch `DirectX12TriangleHelloWorld.exe` from that folder and capture via the RenderDoc UI.

## How it works
- `d3d12.dll` forwards non-hooked exports to the system `d3d12.dll`.
- Importing `helper.dll!DllCanUnloadNow` forces the helper to load.
- `helper.dll` waits for `renderdoc.dll` to already be loaded, then calls `RENDERDOC_GetAPI`.
- The helper only runs when `D3D12_HOOK_RENDERDOC=1` is set.

## Known limitation (no DXGI proxy)
This project does **not** wrap `dxgi.dll`/`dxcore.dll`. If RenderDoc is injected after the DXGI factory is created, you can
end up with mismatched objects (unwrapped DXGI factory + wrapped D3D12 command queue), which can cause swapchain creation
errors. The recommended workflow is to let RenderDoc inject at process start (Launch Application) so DXGI and D3D12 are wrapped
consistently.
