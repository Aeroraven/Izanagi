#include <d3d11.h>
#include <dxgi.h>
#include <windows.h>
#include <cstdarg>
#include <cstdio>
#include <string>

extern "C" void __stdcall HelperInitialize();
extern "C" void __stdcall HelperLog(const char *message);
extern "C" void __stdcall HelperWrapDevice(ID3D11Device **device);

#define X(name) extern "C" void *g_##name = nullptr;
#include "d3d11_stub_exports.inc"
#undef X

namespace {
HMODULE g_realD3D11 = nullptr;
INIT_ONCE g_loadOnce = INIT_ONCE_STATIC_INIT;
INIT_ONCE g_exportsOnce = INIT_ONCE_STATIC_INIT;

BOOL CALLBACK LoadRealD3D11Once(PINIT_ONCE, PVOID, PVOID *) {
  wchar_t systemDir[MAX_PATH] = {};
  const UINT len = GetSystemDirectoryW(systemDir, MAX_PATH);
  if (len > 0 && len < MAX_PATH) {
    std::wstring path(systemDir);
    path += L"\\d3d11.dll";
    g_realD3D11 = LoadLibraryW(path.c_str());
  }
  return TRUE;
}

void EnsureRealD3D11Loaded() {
  InitOnceExecuteOnce(&g_loadOnce, LoadRealD3D11Once, nullptr, nullptr);
}

BOOL CALLBACK LoadExportsOnce(PINIT_ONCE, PVOID, PVOID *) {
  EnsureRealD3D11Loaded();
  if (!g_realD3D11) {
    return TRUE;
  }
#define X(name) g_##name = reinterpret_cast<void *>(GetProcAddress(g_realD3D11, #name));
#include "d3d11_stub_exports.inc"
#undef X
  return TRUE;
}

void EnsureExportsLoaded() {
  InitOnceExecuteOnce(&g_exportsOnce, LoadExportsOnce, nullptr, nullptr);
}

extern "C" void __stdcall EnsureExportsLoadedForStubs() {
  EnsureExportsLoaded();
}

template <typename T> T GetRealProc(const char *name) {
  EnsureRealD3D11Loaded();
  if (!g_realD3D11) {
    return nullptr;
  }
  return reinterpret_cast<T>(GetProcAddress(g_realD3D11, name));
}

HRESULT MissingExport() { return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND); }

void LogFormat(const char *fmt, ...) {
  if (!fmt) {
    return;
  }
  char buffer[512] = {};
  va_list args;
  va_start(args, fmt);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, args);
  va_end(args);
  HelperLog(buffer);
}
} // namespace

extern "C" HRESULT WINAPI Proxy_D3D11CreateDevice(
    IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType,
    HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels, UINT SDKVersion, ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel, ID3D11DeviceContext **ppImmediateContext) {
  using Fn = HRESULT(WINAPI *)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT,
                               const D3D_FEATURE_LEVEL *, UINT, UINT,
                               ID3D11Device **, D3D_FEATURE_LEVEL *,
                               ID3D11DeviceContext **);
  HelperInitialize();
  auto fn = GetRealProc<Fn>("D3D11CreateDevice");
  const HRESULT hr = fn ? fn(pAdapter, DriverType, Software, Flags,
                             pFeatureLevels, FeatureLevels, SDKVersion,
                             ppDevice, pFeatureLevel, ppImmediateContext)
                        : MissingExport();
  LogFormat("D3D11CreateDevice hr=0x%08X driver=%u flags=0x%X",
            static_cast<unsigned>(hr), static_cast<unsigned>(DriverType),
            static_cast<unsigned>(Flags));
  if (SUCCEEDED(hr) && ppDevice && *ppDevice) {
    HelperWrapDevice(ppDevice);
  }
  return hr;
}

extern "C" HRESULT WINAPI Proxy_D3D11CreateDeviceAndSwapChain(
    IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType,
    HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels, UINT SDKVersion,
    const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
    IDXGISwapChain **ppSwapChain, ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel, ID3D11DeviceContext **ppImmediateContext) {
  using Fn = HRESULT(WINAPI *)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT,
                               const D3D_FEATURE_LEVEL *, UINT, UINT,
                               const DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **,
                               ID3D11Device **, D3D_FEATURE_LEVEL *,
                               ID3D11DeviceContext **);
  HelperInitialize();
  auto fn = GetRealProc<Fn>("D3D11CreateDeviceAndSwapChain");
  const HRESULT hr = fn ? fn(pAdapter, DriverType, Software, Flags,
                             pFeatureLevels, FeatureLevels, SDKVersion,
                             pSwapChainDesc, ppSwapChain, ppDevice,
                             pFeatureLevel, ppImmediateContext)
                        : MissingExport();
  if (pSwapChainDesc) {
    LogFormat("D3D11CreateDeviceAndSwapChain hr=0x%08X %ux%u format=%u",
              static_cast<unsigned>(hr),
              static_cast<unsigned>(pSwapChainDesc->BufferDesc.Width),
              static_cast<unsigned>(pSwapChainDesc->BufferDesc.Height),
              static_cast<unsigned>(pSwapChainDesc->BufferDesc.Format));
  } else {
    LogFormat("D3D11CreateDeviceAndSwapChain hr=0x%08X", static_cast<unsigned>(hr));
  }
  if (SUCCEEDED(hr) && ppDevice && *ppDevice) {
    HelperWrapDevice(ppDevice);
  }
  return hr;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(instance);
  }
  return TRUE;
}
