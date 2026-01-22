#include "renderdoc_app.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <windows.h>

namespace {
INIT_ONCE g_consoleOnce = INIT_ONCE_STATIC_INIT;
INIT_ONCE g_initOnce = INIT_ONCE_STATIC_INIT;
HMODULE g_renderdoc = nullptr;
RENDERDOC_API_1_6_0 *g_rdocApi = nullptr;
RENDERDOC_DevicePointer g_device = nullptr;
std::atomic_bool g_activeWindowSet{false};
std::atomic_bool g_activeWindowThreadStarted{false};
std::atomic_bool g_renderdocReady{false};
std::atomic_bool g_renderdocDetected{false};
std::atomic_bool g_getApiStarted{false};
const RENDERDOC_InputButton kCaptureKeys[] = {eRENDERDOC_Key_F12};

BOOL CALLBACK InitConsoleOnce(PINIT_ONCE, PVOID, PVOID *) {
  if (AllocConsole()) {
    FILE *out = nullptr;
    FILE *err = nullptr;
    freopen_s(&out, "CONOUT$", "w", stdout);
    freopen_s(&err, "CONOUT$", "w", stderr);
    SetConsoleTitleW(L"D3D12Hooking");
  }
  return TRUE;
}

void EnsureConsole() {
  InitOnceExecuteOnce(&g_consoleOnce, InitConsoleOnce, nullptr, nullptr);
}

void LogLine(const char *msg) {
  EnsureConsole();
  if (msg) {
    printf("%s\n", msg);
    fflush(stdout);
  }
}

struct WindowSearch {
  DWORD pid;
  HWND hwnd;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam) {
  auto *state = reinterpret_cast<WindowSearch *>(lparam);
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (pid != state->pid) {
    return TRUE;
  }
  if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER) != nullptr) {
    return TRUE;
  }
  state->hwnd = hwnd;
  return FALSE;
}

HWND FindMainWindow() {
  WindowSearch state{};
  state.pid = GetCurrentProcessId();
  state.hwnd = nullptr;
  EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&state));
  return state.hwnd;
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

bool IsGetApiEnabled() {
  char buffer[8] = {};
  const DWORD len = GetEnvironmentVariableA("D3D12_HOOK_GETAPI", buffer,
                                            static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return false;
  }
  return buffer[0] == '1' || buffer[0] == 'y' || buffer[0] == 'Y';
}

bool IsGetApiWaitDeviceEnabled() {
  char buffer[8] = {};
  const DWORD len =
      GetEnvironmentVariableA("D3D12_HOOK_GETAPI_WAIT_DEVICE", buffer,
                              static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return true;
  }
  return buffer[0] == '1' || buffer[0] == 'y' || buffer[0] == 'Y';
}

DWORD GetGetApiDelayMs() {
  char buffer[16] = {};
  const DWORD len = GetEnvironmentVariableA(
      "D3D12_HOOK_GETAPI_DELAY_MS", buffer, static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return 1500;
  }
  char *end = nullptr;
  const unsigned long value = strtoul(buffer, &end, 10);
  if (end == buffer) {
    return 1500;
  }
  return static_cast<DWORD>(value);
}

bool IsAutoCaptureEnabled() {
  char buffer[8] = {};
  const DWORD len = GetEnvironmentVariableA("D3D12_HOOK_CAPTURE", buffer,
                                            static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return false;
  }
  return buffer[0] == '1' || buffer[0] == 'y' || buffer[0] == 'Y';
}

bool IsOverlayEnabled() {
  char buffer[8] = {};
  const DWORD len = GetEnvironmentVariableA("D3D12_HOOK_OVERLAY", buffer,
                                            static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return false;
  }
  return buffer[0] == '1' || buffer[0] == 'y' || buffer[0] == 'Y';
}

bool IsCaptureKeysEnabled() {
  char buffer[8] = {};
  const DWORD len = GetEnvironmentVariableA("D3D12_HOOK_KEYS", buffer,
                                            static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return false;
  }
  return buffer[0] == '1' || buffer[0] == 'y' || buffer[0] == 'Y';
}

bool IsActiveWindowEnabled() {
  char buffer[8] = {};
  const DWORD len = GetEnvironmentVariableA("D3D12_HOOK_ACTIVE_WINDOW", buffer,
                                            static_cast<DWORD>(sizeof(buffer)));
  if (len == 0 || len >= sizeof(buffer)) {
    return false;
  }
  return buffer[0] == '1' || buffer[0] == 'y' || buffer[0] == 'Y';
}

bool TrySetActiveWindow() {
  if (g_activeWindowSet.load()) {
    return true;
  }
  if (!g_rdocApi || !g_device) {
    return false;
  }
  HWND hwnd = FindMainWindow();
  if (!hwnd) {
    return false;
  }
  g_rdocApi->SetActiveWindow(g_device, hwnd);
  g_activeWindowSet.store(true);
  LogLine("D3D12Hooking: RenderDoc active window set.");
  if (IsOverlayEnabled()) {
    g_rdocApi->MaskOverlayBits(0, eRENDERDOC_Overlay_All);
    LogLine("D3D12Hooking: RenderDoc overlay enabled.");
  }
  return true;
}

DWORD WINAPI ActiveWindowWatcherThread(LPVOID) {
  for (int i = 0; i < 50; ++i) {
    if (TrySetActiveWindow()) {
      return 0;
    }
    Sleep(200);
  }
  LogLine("D3D12Hooking: RenderDoc active window not set.");
  g_activeWindowThreadStarted.store(false);
  return 0;
}

void EnsureActiveWindowWatcher() {
  if (g_activeWindowThreadStarted.exchange(true)) {
    return;
  }
  HANDLE thread =
      CreateThread(nullptr, 0, ActiveWindowWatcherThread, nullptr, 0, nullptr);
  if (thread) {
    CloseHandle(thread);
  }
}

bool TryInitRenderDoc() {
  g_renderdoc = GetModuleHandleW(L"renderdoc.dll");
  if (!g_renderdoc) {
    return false;
  }

  LogLine("D3D12Hooking: renderdoc.dll detected.");
  g_renderdocDetected.store(true);
  return true;
}

DWORD WINAPI GetApiThread(LPVOID) {
  const DWORD delayMs = GetGetApiDelayMs();
  if (delayMs > 0) {
    Sleep(delayMs);
  }

  if (IsGetApiWaitDeviceEnabled()) {
    for (int i = 0; i < 80; ++i) {
      if (g_device) {
        break;
      }
      Sleep(50);
    }
  }
  LogLine("D3D12Hooking: loading renderdoc API...");
  if (!g_renderdoc) {
    g_renderdoc = GetModuleHandleW(L"renderdoc.dll");
    if (!g_renderdoc) {
      LogLine("D3D12Hooking: renderdoc.dll not present for GetAPI.");
      return 0;
    }
  }

  auto getApi = reinterpret_cast<pRENDERDOC_GetAPI>(
      GetProcAddress(g_renderdoc, "RENDERDOC_GetAPI"));
  if (!getApi) {
    LogLine("D3D12Hooking: renderdoc.dll missing RENDERDOC_GetAPI.");
    return 0;
  }

  void *api = nullptr;
  if (!getApi(eRENDERDOC_API_Version_1_6_0, &api)) {
    LogLine("D3D12Hooking: RenderDoc API init failed.");
    return 0;
  }

  g_rdocApi = reinterpret_cast<RENDERDOC_API_1_6_0 *>(api);
  if (IsCaptureKeysEnabled()) {
    g_rdocApi->SetCaptureKeys(const_cast<RENDERDOC_InputButton *>(kCaptureKeys),
                              1);
  }
  LogLine("D3D12Hooking: RenderDoc API initialized.");
  g_renderdocReady.store(true);
  if (IsActiveWindowEnabled()) {
    EnsureActiveWindowWatcher();
  }
  return 0;
}

void TryStartGetApi() {
  if (!IsGetApiEnabled()) {
    return;
  }
  if (!g_renderdocDetected.load()) {
    return;
  }
  if (g_getApiStarted.exchange(true)) {
    return;
  }
  HANDLE thread = CreateThread(nullptr, 0, GetApiThread, nullptr, 0, nullptr);
  if (thread) {
    CloseHandle(thread);
  }
}

DWORD WINAPI RenderDocWatcherThread(LPVOID) {
  LogLine("D3D12Hooking: Waiting for renderdoc.dll to be injected...");
  for (int i = 0; i < 50; ++i) {
    if (TryInitRenderDoc()) {
      TryStartGetApi();
      if (IsAutoCaptureEnabled() && g_rdocApi) {
        Sleep(1000);
        RENDERDOC_DevicePointer device = nullptr;
        HWND hwnd = nullptr;
        for (int j = 0; j < 50; ++j) {
          device = g_device;
          hwnd = FindMainWindow();
          if (device && hwnd) {
            break;
          }
          Sleep(100);
        }
        LogLine("D3D12Hooking: Starting frame capture.");
        g_rdocApi->StartFrameCapture(device, hwnd);
        Sleep(200);
        const uint32_t ok = g_rdocApi->EndFrameCapture(device, hwnd);
        char msg[128];
        sprintf_s(msg, "D3D12Hooking: EndFrameCapture returned %u.", ok);
        LogLine(msg);
        const uint32_t num = g_rdocApi->GetNumCaptures();
        sprintf_s(msg, "D3D12Hooking: Captures recorded = %u.", num);
        LogLine(msg);
      }
      return 0;
    }
    Sleep(200);
  }

  LogLine("D3D12Hooking: renderdoc.dll not injected; skipping.");
  return 0;
}

BOOL CALLBACK StartWatcherOnce(PINIT_ONCE, PVOID, PVOID *) {
  if (!IsRenderDocEnabled()) {
    return TRUE;
  }
  HANDLE thread =
      CreateThread(nullptr, 0, RenderDocWatcherThread, nullptr, 0, nullptr);
  if (thread) {
    CloseHandle(thread);
  }
  return TRUE;
}

void EnsureRenderDocLoaded() {
  InitOnceExecuteOnce(&g_initOnce, StartWatcherOnce, nullptr, nullptr);
}
} // namespace

extern "C" __declspec(dllexport) HRESULT __stdcall DllCanUnloadNow() {
  return S_FALSE;
}

extern "C"
    __declspec(dllexport) HRESULT __stdcall DllGetClassObject(REFCLSID, REFIID,
                                                              LPVOID *) {
  return CLASS_E_CLASSNOTAVAILABLE;
}

extern "C" __declspec(dllexport) void __stdcall HelperInitialize() {
  EnsureRenderDocLoaded();
}

extern "C" __declspec(dllexport) void __stdcall HelperSetDevice(void *device) {
  g_device = device;
  TryStartGetApi();
  if (g_rdocApi && IsActiveWindowEnabled()) {
    EnsureActiveWindowWatcher();
  }
}

extern "C" __declspec(dllexport) BOOL __stdcall HelperIsRenderDocReady() {
  return g_renderdocReady.load() ? TRUE : FALSE;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(instance);
  }
  return TRUE;
}
