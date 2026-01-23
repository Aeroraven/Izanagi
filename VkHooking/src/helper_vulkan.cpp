#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shlobj.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <filesystem>
#include <mutex>
#include <string>

namespace {
constexpr wchar_t kRootFolderName[] = L"Izanagi_Logs";

INIT_ONCE g_initOnce = INIT_ONCE_STATIC_INIT;
std::filesystem::path g_rootDir;
std::filesystem::path g_logDir;
std::filesystem::path g_logFile;
std::mutex g_logMutex;
std::atomic<uint32_t> g_shaderId{0};
std::atomic_bool g_ready{false};

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

void AppendLogLine(const std::string &line) {
  if (!g_ready.load()) {
    return;
  }
  std::lock_guard<std::mutex> lock(g_logMutex);
  FILE *file = nullptr;
  if (_wfopen_s(&file, g_logFile.c_str(), L"ab") != 0 || !file) {
    return;
  }
  fwrite(line.data(), 1, line.size(), file);
  fwrite("\n", 1, 1, file);
  fclose(file);
}

void LogFormat(const char *fmt, ...) {
  if (!fmt) {
    return;
  }
  char buffer[512] = {};
  va_list args;
  va_start(args, fmt);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, args);
  va_end(args);
  AppendLogLine(buffer);
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
    AppendLogLine("VkHooking: session started");
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
  LogFormat("VkHooking: saved shader %ls (%zu bytes)", filename.c_str(), size);
}
} // namespace

extern "C" __declspec(dllexport) void __stdcall HelperInitialize() {
  EnsureInitialized();
}

extern "C" __declspec(dllexport) void __stdcall HelperLog(const char *message) {
  EnsureInitialized();
  if (message) {
    AppendLogLine(message);
  }
}

extern "C" __declspec(dllexport) void __stdcall
HelperSaveShaderIR(const void *data, size_t size) {
  SaveShader(data, size);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(instance);
  }
  return TRUE;
}
