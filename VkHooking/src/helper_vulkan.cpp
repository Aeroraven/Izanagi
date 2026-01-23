#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shlobj.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <string>
#include <cctype>

#include <dbghelp.h>

namespace {
constexpr wchar_t kRootFolderName[] = L"Izanagi_Logs";

INIT_ONCE g_initOnce = INIT_ONCE_STATIC_INIT;
INIT_ONCE g_dbgHelpOnce = INIT_ONCE_STATIC_INIT;
INIT_ONCE g_consoleOnce = INIT_ONCE_STATIC_INIT;
std::filesystem::path g_rootDir;
std::filesystem::path g_logDir;
std::filesystem::path g_logFile;
std::mutex g_logMutex;
std::atomic<uint32_t> g_shaderId{0};
std::atomic<uint64_t> g_frameId{0};
std::atomic<uint32_t> g_pendingConsoleLogs{0};
std::atomic_bool g_ready{false};
std::atomic_bool g_consoleThreadStarted{false};
HANDLE g_process = GetCurrentProcess();

BOOL CALLBACK InitDbgHelpOnce(PINIT_ONCE, PVOID, PVOID *) {
  SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
  SymInitialize(g_process, nullptr, TRUE);
  return TRUE;
}

BOOL CALLBACK InitConsoleOnce(PINIT_ONCE, PVOID, PVOID *) {
  if (!GetConsoleWindow()) {
    AllocConsole();
    FILE *in = nullptr;
    FILE *out = nullptr;
    freopen_s(&in, "CONIN$", "r", stdin);
    freopen_s(&out, "CONOUT$", "w", stdout);
    SetConsoleTitleW(L"VkHooking");
  }
  return TRUE;
}

std::wstring GetDocumentsPath() {
  PWSTR path = nullptr;
  if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT,
                                     nullptr, &path))) {
    std::wstring result(path);
    CoTaskMemFree(path);
    return result;
  }

  wchar_t fallback[MAX_PATH] = {};
  if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PERSONAL, nullptr,
                                 SHGFP_TYPE_CURRENT, fallback))) {
    return fallback;
  }

  wchar_t profile[MAX_PATH] = {};
  const DWORD len = GetEnvironmentVariableW(L"USERPROFILE", profile, MAX_PATH);
  if (len > 0 && len < MAX_PATH) {
    std::wstring result(profile);
    result += L"\\Documents";
    return result;
  }
  return {};
}

std::string BuildTimestamp() {
  SYSTEMTIME st = {};
  GetLocalTime(&st);
  char buffer[32] = {};
  sprintf_s(buffer, "%02u:%02u:%02u.%03u", st.wHour, st.wMinute, st.wSecond,
            st.wMilliseconds);
  return std::string(buffer);
}

void AppendLogLineFormatted(const char *api, const char *message) {
  if (!g_ready.load()) {
    return;
  }
  const std::string timestamp = BuildTimestamp();
  const uint64_t frame = g_frameId.load();
  const char *apiName = (api && *api) ? api : "Unknown";
  const char *msg = (message && *message) ? message : "";
  char line[1024] = {};
  sprintf_s(line, "[%s][%llu][%s]: %s", timestamp.c_str(),
            static_cast<unsigned long long>(frame), apiName, msg);
  std::lock_guard<std::mutex> lock(g_logMutex);
  FILE *file = nullptr;
  if (_wfopen_s(&file, g_logFile.c_str(), L"ab") != 0 || !file) {
    return;
  }
  const size_t len = strlen(line);
  fwrite(line, 1, len, file);
  fwrite("\n", 1, 1, file);
  fclose(file);
}

void LogFormat(const char *api, const char *fmt, ...) {
  if (!fmt) {
    return;
  }
  char buffer[512] = {};
  va_list args;
  va_start(args, fmt);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, args);
  va_end(args);
  AppendLogLineFormatted(api, buffer);
}

BOOL CALLBACK InitOnceProc(PINIT_ONCE, PVOID, PVOID *) {
  const std::wstring docs = GetDocumentsPath();
  if (docs.empty()) {
    return TRUE;
  }

  SYSTEMTIME st = {};
  GetLocalTime(&st);
  wchar_t stamp[64] = {};
  swprintf_s(stamp, L"%04u%02u%02u-%02u%02u%02u-vkhook", st.wYear, st.wMonth,
             st.wDay, st.wHour, st.wMinute, st.wSecond);

  std::filesystem::path base(docs);
  base /= kRootFolderName;
  std::error_code ec;
  std::filesystem::create_directories(base, ec);
  g_rootDir = base / stamp;
  g_logDir = g_rootDir / L"log";
  std::filesystem::create_directories(g_logDir, ec);
  g_logFile = g_logDir / L"vkhook.log";
  g_ready.store(!g_rootDir.empty());
  if (g_ready.load()) {
    AppendLogLineFormatted("Helper", "session started");
  }
  return TRUE;
}

void EnsureInitialized() {
  InitOnceExecuteOnce(&g_initOnce, InitOnceProc, nullptr, nullptr);
}

enum class ShaderStage {
  Unknown,
  Vertex,
  Fragment,
  Compute,
  Geometry,
  TessControl,
  TessEvaluation,
  Task,
  Mesh
};

ShaderStage GetStageFromSpirv(const void *data, size_t size) {
  if (!data || size < 20 || (size % 4) != 0) {
    return ShaderStage::Unknown;
  }
  const uint32_t *words = static_cast<const uint32_t *>(data);
  const size_t wordCount = size / 4;
  if (words[0] != 0x07230203u || wordCount < 6) {
    return ShaderStage::Unknown;
  }
  size_t i = 5;
  while (i < wordCount) {
    const uint32_t op = words[i] & 0xFFFFu;
    const uint32_t count = words[i] >> 16u;
    if (count == 0 || i + count > wordCount) {
      break;
    }
    if (op == 15u && count >= 3u) {
      const uint32_t model = words[i + 1];
      switch (model) {
      case 0u:
        return ShaderStage::Vertex;
      case 1u:
        return ShaderStage::TessControl;
      case 2u:
        return ShaderStage::TessEvaluation;
      case 3u:
        return ShaderStage::Geometry;
      case 4u:
        return ShaderStage::Fragment;
      case 5u:
        return ShaderStage::Compute;
      case 7u:
      case 9u:
        return ShaderStage::Task;
      case 8u:
      case 10u:
        return ShaderStage::Mesh;
      default:
        return ShaderStage::Unknown;
      }
    }
    i += count;
  }
  return ShaderStage::Unknown;
}

const wchar_t *GetStageExtension(ShaderStage stage) {
  switch (stage) {
  case ShaderStage::Vertex:
    return L"vs";
  case ShaderStage::Fragment:
    return L"ps";
  case ShaderStage::Compute:
    return L"cs";
  case ShaderStage::Geometry:
    return L"gs";
  case ShaderStage::TessControl:
    return L"hs";
  case ShaderStage::TessEvaluation:
    return L"ds";
  case ShaderStage::Task:
    return L"ts";
  case ShaderStage::Mesh:
    return L"ms";
  default:
    return L"spv";
  }
}

void SaveShader(const void *data, size_t size) {
  if (!data || size == 0) {
    return;
  }
  EnsureInitialized();
  if (!g_ready.load()) {
    return;
  }
  const ShaderStage stage = GetStageFromSpirv(data, size);
  const wchar_t *ext = GetStageExtension(stage);
  const uint32_t id = g_shaderId.fetch_add(1) + 1;
  std::wstring filename = std::to_wstring(id);
  filename += L".";
  filename += ext;
  const auto path = g_rootDir / filename;

  FILE *file = nullptr;
  if (_wfopen_s(&file, path.c_str(), L"wb") != 0 || !file) {
    return;
  }
  fwrite(data, 1, size, file);
  fclose(file);
  LogFormat("HelperSaveShaderIR", "saved shader %ls (%zu bytes)",
            filename.c_str(), size);
}

void EnsureDbgHelp() {
  InitOnceExecuteOnce(&g_dbgHelpOnce, InitDbgHelpOnce, nullptr, nullptr);
}

void EnsureConsole() {
  InitOnceExecuteOnce(&g_consoleOnce, InitConsoleOnce, nullptr, nullptr);
}

DWORD WINAPI ConsoleThread(LPVOID) {
  EnsureConsole();
  char buffer[256] = {};
  while (true) {
    if (!fgets(buffer, sizeof(buffer), stdin)) {
      Sleep(50);
      clearerr(stdin);
      continue;
    }
    char *p = buffer;
    while (*p && std::isspace(static_cast<unsigned char>(*p))) {
      ++p;
    }
    if (*p == 'c' || *p == 'C') {
      ++p;
      while (*p && std::isspace(static_cast<unsigned char>(*p))) {
        ++p;
      }
      if (*p == '\0') {
        g_pendingConsoleLogs.fetch_add(1);
      }
    }
  }
  return 0;
}

void AppendCallstack(const char *api) {
  EnsureDbgHelp();
  void *frames[64] = {};
  const USHORT captured = CaptureStackBackTrace(2, 64, frames, nullptr);
  if (captured == 0) {
    AppendLogLineFormatted(api, "  at <no stack>");
    return;
  }

  for (USHORT i = 0; i < captured; ++i) {
    const DWORD64 addr = reinterpret_cast<DWORD64>(frames[i]);
    DWORD64 displacement = 0;
    char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
    auto *symbol = reinterpret_cast<SYMBOL_INFO *>(symbolBuffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    char lineBuffer[1024] = {};
    if (SymFromAddr(g_process, addr, &displacement, symbol)) {
      IMAGEHLP_LINE64 line = {};
      line.SizeOfStruct = sizeof(line);
      DWORD lineDisplacement = 0;
      if (SymGetLineFromAddr64(g_process, addr, &lineDisplacement, &line)) {
        sprintf_s(lineBuffer, "  at %s (%s:%lu +0x%llX)", symbol->Name,
                  line.FileName, line.LineNumber,
                  static_cast<unsigned long long>(displacement));
      } else {
        sprintf_s(lineBuffer, "  at %s (+0x%llX)", symbol->Name,
                  static_cast<unsigned long long>(displacement));
      }
    } else {
      sprintf_s(lineBuffer, "  at 0x%llX",
                static_cast<unsigned long long>(addr));
    }
    AppendLogLineFormatted(api, lineBuffer);
  }
}

void FlushConsoleLogs() {
  const uint32_t count = g_pendingConsoleLogs.exchange(0);
  for (uint32_t i = 0; i < count; ++i) {
    AppendLogLineFormatted("Console", "input c");
  }
}
} // namespace

extern "C" __declspec(dllexport) void __stdcall HelperInitialize() {
  EnsureInitialized();
}

extern "C" __declspec(dllexport) void __stdcall HelperLog(const char *message) {
  EnsureInitialized();
  AppendLogLineFormatted("Helper", message);
}

extern "C" __declspec(dllexport) void __stdcall
HelperSaveShaderIR(const void *data, size_t size) {
  SaveShader(data, size);
}

extern "C" __declspec(dllexport) void __stdcall HelperAdvanceFrame() {
  g_frameId.fetch_add(1);
}

extern "C" __declspec(dllexport) void __stdcall
HelperLogApi(const char *api, const char *message) {
  EnsureInitialized();
  AppendLogLineFormatted(api, message);
}

extern "C" __declspec(dllexport) void __stdcall
HelperLogCallstack(const char *api, const char *message) {
  EnsureInitialized();
  AppendLogLineFormatted(api, message);
  AppendCallstack(api);
}

extern "C" __declspec(dllexport) void __stdcall HelperStartConsole() {
  if (g_consoleThreadStarted.exchange(true)) {
    return;
  }
  HANDLE thread = CreateThread(nullptr, 0, ConsoleThread, nullptr, 0, nullptr);
  if (thread) {
    CloseHandle(thread);
  }
}

extern "C" __declspec(dllexport) void __stdcall HelperOnFrameStart() {
  EnsureInitialized();
  FlushConsoleLogs();
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(instance);
  }
  return TRUE;
}
