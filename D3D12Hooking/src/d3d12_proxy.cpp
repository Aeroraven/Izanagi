#include <d3d12.h>
#include <d3dcommon.h>
#include <iostream>
#include <string>
#include <windows.h>
extern "C" void __stdcall HelperInitialize();

extern "C" void *g_ord99 = nullptr;
extern "C" void *g_SetAppCompatStringPointer = nullptr;
extern "C" void *g_D3D12CoreCreateLayeredDevice = nullptr;
extern "C" void *g_D3D12CoreGetLayeredDeviceSize = nullptr;
extern "C" void *g_D3D12CoreRegisterLayers = nullptr;
extern "C" void *g_D3D12DeviceRemovedExtendedData = nullptr;
extern "C" void *g_D3D12GetInterface = nullptr;
extern "C" void *g_D3D12PIXEventsReplaceBlock = nullptr;
extern "C" void *g_D3D12PIXGetThreadInfo = nullptr;
extern "C" void *g_D3D12PIXNotifyWakeFromFenceSignal = nullptr;
extern "C" void *g_D3D12PIXReportCounter = nullptr;
extern "C" void *g_GetBehaviorValue = nullptr;
extern "C" void __stdcall EnsureExportsLoadedForStubs();
extern "C" void __stdcall HelperSetDevice(void *device);
extern "C" void __stdcall HelperInitialize();
extern "C" BOOL __stdcall HelperIsRenderDocReady();

namespace {
HMODULE g_realD3D12 = nullptr;
INIT_ONCE g_loadOnce = INIT_ONCE_STATIC_INIT;
INIT_ONCE g_exportsOnce = INIT_ONCE_STATIC_INIT;
INIT_ONCE g_renderdocOnce = INIT_ONCE_STATIC_INIT;

BOOL CALLBACK LoadRealD3D12Once(PINIT_ONCE, PVOID, PVOID *) {
  wchar_t systemDir[MAX_PATH] = {};
  const UINT len = GetSystemDirectoryW(systemDir, MAX_PATH);
  if (len > 0 && len < MAX_PATH) {
    std::wstring path(systemDir);
    path += L"\\d3d12.dll";
    g_realD3D12 = LoadLibraryW(path.c_str());
  }
  return TRUE;
}

void EnsureRealD3D12Loaded() {
  std::cout << "Loading real d3d12.dll" << std::endl;
  InitOnceExecuteOnce(&g_loadOnce, LoadRealD3D12Once, nullptr, nullptr);
}

BOOL CALLBACK LoadExportsOnce(PINIT_ONCE, PVOID, PVOID *) {
  EnsureRealD3D12Loaded();
  if (!g_realD3D12) {
    return TRUE;
  }

  g_ord99 = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, MAKEINTRESOURCEA(99)));
  g_SetAppCompatStringPointer = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "SetAppCompatStringPointer"));
  g_D3D12CoreCreateLayeredDevice = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "D3D12CoreCreateLayeredDevice"));
  g_D3D12CoreGetLayeredDeviceSize = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "D3D12CoreGetLayeredDeviceSize"));
  g_D3D12CoreRegisterLayers = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "D3D12CoreRegisterLayers"));
  g_D3D12DeviceRemovedExtendedData = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "D3D12DeviceRemovedExtendedData"));
  g_D3D12GetInterface = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "D3D12GetInterface"));
  g_D3D12PIXEventsReplaceBlock = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "D3D12PIXEventsReplaceBlock"));
  g_D3D12PIXGetThreadInfo = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "D3D12PIXGetThreadInfo"));
  g_D3D12PIXNotifyWakeFromFenceSignal = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "D3D12PIXNotifyWakeFromFenceSignal"));
  g_D3D12PIXReportCounter = reinterpret_cast<void *>(
      GetProcAddress(g_realD3D12, "D3D12PIXReportCounter"));
  g_GetBehaviorValue =
      reinterpret_cast<void *>(GetProcAddress(g_realD3D12, "GetBehaviorValue"));
  return TRUE;
}

void EnsureExportsLoaded() {
  InitOnceExecuteOnce(&g_exportsOnce, LoadExportsOnce, nullptr, nullptr);
}

extern "C" void __stdcall EnsureExportsLoadedForStubs() {
  EnsureExportsLoaded();
}

BOOL CALLBACK InitRenderDocOnce(PINIT_ONCE, PVOID, PVOID *) {
  HelperInitialize();
  return TRUE;
}

void EnsureRenderDocInitialized() {
  InitOnceExecuteOnce(&g_renderdocOnce, InitRenderDocOnce, nullptr, nullptr);
}

bool IsRenderDocEnabled() {
  char buffer[8] = {};
  const DWORD len = GetEnvironmentVariableA("D3D12_HOOK_RENDERDOC", buffer,
                                            static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return false;
  }
  return buffer[0] == '1' || buffer[0] == 'y' || buffer[0] == 'Y';
}

bool ShouldWaitForRenderDoc() {
  char buffer[8] = {};
  const DWORD len = GetEnvironmentVariableA("D3D12_HOOK_WAIT_RENDERDOC", buffer,
                                            static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return false;
  }
  return buffer[0] == '1' || buffer[0] == 'y' || buffer[0] == 'Y';
}

DWORD GetRenderDocWaitMs() {
  char buffer[16] = {};
  const DWORD len = GetEnvironmentVariableA("D3D12_HOOK_WAIT_MS", buffer,
                                            static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return 5000;
  }
  char *end = nullptr;
  const unsigned long value = strtoul(buffer, &end, 10);
  if (end == buffer || value == 0) {
    return 5000;
  }
  return static_cast<DWORD>(value);
}

void WaitForRenderDocInjected() {
  if (!ShouldWaitForRenderDoc()) {
    return;
  }
  const DWORD waitMs = GetRenderDocWaitMs();
  const DWORD start = GetTickCount();
  while (GetTickCount() - start < waitMs) {
    if (GetModuleHandleW(L"renderdoc.dll") != nullptr) {
      return;
    }
    Sleep(10);
  }
}

template <typename T> T GetRealProc(const char *name) {
  EnsureRealD3D12Loaded();
  if (!g_realD3D12) {
    return nullptr;
  }
  return reinterpret_cast<T>(GetProcAddress(g_realD3D12, name));
}

HRESULT MissingExport() { return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND); }
} // namespace

extern "C" HRESULT WINAPI Proxy_D3D12CreateDevice(
    IUnknown *pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, REFIID riid,
    void **ppDevice) {
  using Fn = HRESULT(WINAPI *)(IUnknown *, D3D_FEATURE_LEVEL, REFIID, void **);
  if (IsRenderDocEnabled()) {
    EnsureRenderDocInitialized();
    WaitForRenderDocInjected();
  }
  auto fn = GetRealProc<Fn>("D3D12CreateDevice");
  const HRESULT hr =
      fn ? fn(pAdapter, MinimumFeatureLevel, riid, ppDevice) : MissingExport();
  if (SUCCEEDED(hr)) {
    if (ppDevice && *ppDevice) {
      HelperSetDevice(*ppDevice);
    }
  }
  return hr;
}

extern "C" HRESULT WINAPI Proxy_D3D12GetDebugInterface(REFIID riid,
                                                       void **ppvDebug) {
  using Fn = HRESULT(WINAPI *)(REFIID, void **);
  auto fn = GetRealProc<Fn>("D3D12GetDebugInterface");
  return fn ? fn(riid, ppvDebug) : MissingExport();
}

extern "C" HRESULT WINAPI Proxy_D3D12SerializeRootSignature(
    const D3D12_ROOT_SIGNATURE_DESC *pRootSignature,
    D3D_ROOT_SIGNATURE_VERSION Version, ID3DBlob **ppBlob,
    ID3DBlob **ppErrorBlob) {
  using Fn =
      HRESULT(WINAPI *)(const D3D12_ROOT_SIGNATURE_DESC *,
                        D3D_ROOT_SIGNATURE_VERSION, ID3DBlob **, ID3DBlob **);
  auto fn = GetRealProc<Fn>("D3D12SerializeRootSignature");
  return fn ? fn(pRootSignature, Version, ppBlob, ppErrorBlob)
            : MissingExport();
}

extern "C" HRESULT WINAPI Proxy_D3D12SerializeVersionedRootSignature(
    const D3D12_VERSIONED_ROOT_SIGNATURE_DESC *pRootSignature,
    ID3DBlob **ppBlob, ID3DBlob **ppErrorBlob) {
  using Fn = HRESULT(WINAPI *)(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC *,
                               ID3DBlob **, ID3DBlob **);
  auto fn = GetRealProc<Fn>("D3D12SerializeVersionedRootSignature");
  return fn ? fn(pRootSignature, ppBlob, ppErrorBlob) : MissingExport();
}

extern "C" HRESULT WINAPI Proxy_D3D12CreateRootSignatureDeserializer(
    LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes, REFIID riid,
    void **ppvDeserializer) {
  using Fn = HRESULT(WINAPI *)(LPCVOID, SIZE_T, REFIID, void **);
  auto fn = GetRealProc<Fn>("D3D12CreateRootSignatureDeserializer");
  return fn ? fn(pSrcData, SrcDataSizeInBytes, riid, ppvDeserializer)
            : MissingExport();
}

extern "C" HRESULT WINAPI Proxy_D3D12CreateVersionedRootSignatureDeserializer(
    LPCVOID pSrcData, SIZE_T SrcDataSizeInBytes, REFIID riid,
    void **ppvDeserializer) {
  using Fn = HRESULT(WINAPI *)(LPCVOID, SIZE_T, REFIID, void **);
  auto fn = GetRealProc<Fn>("D3D12CreateVersionedRootSignatureDeserializer");
  return fn ? fn(pSrcData, SrcDataSizeInBytes, riid, ppvDeserializer)
            : MissingExport();
}

extern "C" HRESULT WINAPI Proxy_D3D12EnableExperimentalFeatures(
    UINT NumFeatures, const IID *pIIDs, void *pConfigurationStructs,
    UINT *pConfigurationStructSizes) {
  using Fn = HRESULT(WINAPI *)(UINT, const IID *, void *, UINT *);
  auto fn = GetRealProc<Fn>("D3D12EnableExperimentalFeatures");
  return fn ? fn(NumFeatures, pIIDs, pConfigurationStructs,
                 pConfigurationStructSizes)
            : MissingExport();
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(instance);
  }
  return TRUE;
}
