#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

extern "C" void __stdcall HelperInitialize();
extern "C" void __stdcall HelperLog(const char *message);
extern "C" void __stdcall HelperSaveShaderIR(const void *data, size_t size);

#define X(name) extern "C" void *g_##name = nullptr;
#include "vulkan_stub_exports.inc"
#undef X

extern "C" void __stdcall EnsureExportsLoadedForStubs();

#define VKHOOK_EXPORT __declspec(dllexport)

extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(
    const VkInstanceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkInstance *pInstance);
extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
    const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties);
extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(
    uint32_t *pPropertyCount, VkLayerProperties *pProperties);
extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(
    uint32_t *pApiVersion);
extern "C" VKHOOK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(
    VkInstance instance, const char *pName);
extern "C" VKHOOK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(
    VkDevice device, const char *pName);
extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(
    VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkDevice *pDevice);
extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(
    VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain);
extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(
    VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo *pCreateInfos,
    const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines);
extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(
    VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
    const VkComputePipelineCreateInfo *pCreateInfos,
    const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines);
extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(
    VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule);
extern "C" VKHOOK_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(
    VkDevice device, VkShaderModule shaderModule,
    const VkAllocationCallbacks *pAllocator);

namespace {
HMODULE g_realVulkan = nullptr;
HMODULE g_selfModule = nullptr;
INIT_ONCE g_loadOnce = INIT_ONCE_STATIC_INIT;
INIT_ONCE g_procOnce = INIT_ONCE_STATIC_INIT;
INIT_ONCE g_exportsOnce = INIT_ONCE_STATIC_INIT;
PFN_vkGetInstanceProcAddr g_realGetInstanceProcAddr = nullptr;
PFN_vkGetDeviceProcAddr g_realGetDeviceProcAddr = nullptr;
VkInstance g_lastInstance = VK_NULL_HANDLE;

BOOL CALLBACK LoadRealVulkanOnce(PINIT_ONCE, PVOID, PVOID *) {
  wchar_t systemDir[MAX_PATH] = {};
  const UINT len = GetSystemDirectoryW(systemDir, MAX_PATH);
  if (len > 0 && len < MAX_PATH) {
    std::wstring path(systemDir);
    path += L"\\vulkan-1.dll";
    g_realVulkan = LoadLibraryW(path.c_str());
  }
  return TRUE;
}

void EnsureRealVulkanLoaded() {
  InitOnceExecuteOnce(&g_loadOnce, LoadRealVulkanOnce, nullptr, nullptr);
}

BOOL CALLBACK LoadProcAddrsOnce(PINIT_ONCE, PVOID, PVOID *) {
  EnsureRealVulkanLoaded();
  if (g_realVulkan) {
    g_realGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        GetProcAddress(g_realVulkan, "vkGetInstanceProcAddr"));
    g_realGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(
        GetProcAddress(g_realVulkan, "vkGetDeviceProcAddr"));
  }
  return TRUE;
}

void EnsureProcAddrsLoaded() {
  InitOnceExecuteOnce(&g_procOnce, LoadProcAddrsOnce, nullptr, nullptr);
}

BOOL CALLBACK LoadExportsOnce(PINIT_ONCE, PVOID, PVOID *) {
  EnsureRealVulkanLoaded();
  if (!g_realVulkan) {
    return TRUE;
  }
#define X(name) g_##name = reinterpret_cast<void *>(GetProcAddress(g_realVulkan, #name));
#include "vulkan_stub_exports.inc"
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
  EnsureRealVulkanLoaded();
  if (!g_realVulkan) {
    return nullptr;
  }
  return reinterpret_cast<T>(GetProcAddress(g_realVulkan, name));
}

PFN_vkVoidFunction GetRealInstanceProc(VkInstance instance, const char *name) {
  EnsureProcAddrsLoaded();
  if (!g_realGetInstanceProcAddr) {
    return nullptr;
  }
  return g_realGetInstanceProcAddr(instance, name);
}

PFN_vkVoidFunction GetRealDeviceProc(VkDevice device, const char *name) {
  EnsureProcAddrsLoaded();
  if (!g_realGetDeviceProcAddr) {
    return nullptr;
  }
  return g_realGetDeviceProcAddr(device, name);
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
  HelperLog(buffer);
}

PFN_vkVoidFunction GetLocalProc(const char *name) {
  if (!name || !g_selfModule) {
    return nullptr;
  }
  FARPROC proc = GetProcAddress(g_selfModule, name);
  if (!proc) {
    return nullptr;
  }
  return reinterpret_cast<PFN_vkVoidFunction>(proc);
}
} // namespace

extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(
    const VkInstanceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkInstance *pInstance) {
  HelperInitialize();
  auto fn = GetRealProc<PFN_vkCreateInstance>("vkCreateInstance");
  const VkResult result =
      fn ? fn(pCreateInfo, pAllocator, pInstance) : VK_ERROR_INITIALIZATION_FAILED;
  if (result == VK_SUCCESS && pInstance) {
    g_lastInstance = *pInstance;
  }
  LogFormat("vkCreateInstance result=%d", static_cast<int>(result));
  return result;
}

extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char *pLayerName,
                                       uint32_t *pPropertyCount,
                                       VkExtensionProperties *pProperties) {
  auto fn = GetRealProc<PFN_vkEnumerateInstanceExtensionProperties>(
      "vkEnumerateInstanceExtensionProperties");
  return fn ? fn(pLayerName, pPropertyCount, pProperties)
            : VK_ERROR_INITIALIZATION_FAILED;
}

extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t *pPropertyCount,
                                   VkLayerProperties *pProperties) {
  auto fn = GetRealProc<PFN_vkEnumerateInstanceLayerProperties>(
      "vkEnumerateInstanceLayerProperties");
  return fn ? fn(pPropertyCount, pProperties) : VK_ERROR_INITIALIZATION_FAILED;
}

extern "C" VKHOOK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceVersion(uint32_t *pApiVersion) {
  auto fn = GetRealProc<PFN_vkEnumerateInstanceVersion>(
      "vkEnumerateInstanceVersion");
  return fn ? fn(pApiVersion) : VK_ERROR_INITIALIZATION_FAILED;
}

extern "C" VKHOOK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance instance, const char *pName) {
  HelperInitialize();
  if (auto local = GetLocalProc(pName)) {
    return local;
  }
  EnsureProcAddrsLoaded();
  return g_realGetInstanceProcAddr ? g_realGetInstanceProcAddr(instance, pName)
                                   : nullptr;
}

extern "C" VKHOOK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetDeviceProcAddr(VkDevice device, const char *pName) {
  HelperInitialize();
  if (auto local = GetLocalProc(pName)) {
    return local;
  }
  EnsureProcAddrsLoaded();
  return g_realGetDeviceProcAddr ? g_realGetDeviceProcAddr(device, pName)
                                 : nullptr;
}

extern "C" VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(
    VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
  HelperInitialize();
  auto fn = reinterpret_cast<PFN_vkCreateDevice>(
      GetRealInstanceProc(g_lastInstance, "vkCreateDevice"));
  if (!fn) {
    fn = GetRealProc<PFN_vkCreateDevice>("vkCreateDevice");
  }
  const VkResult result =
      fn ? fn(physicalDevice, pCreateInfo, pAllocator, pDevice)
         : VK_ERROR_INITIALIZATION_FAILED;
  const uint32_t queues = pCreateInfo ? pCreateInfo->queueCreateInfoCount : 0;
  LogFormat("vkCreateDevice result=%d queues=%u", static_cast<int>(result),
            static_cast<unsigned>(queues));
  return result;
}

extern "C" VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(
    VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain) {
  HelperInitialize();
  auto fn = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
      GetRealDeviceProc(device, "vkCreateSwapchainKHR"));
  const VkResult result =
      fn ? fn(device, pCreateInfo, pAllocator, pSwapchain)
         : VK_ERROR_INITIALIZATION_FAILED;
  if (pCreateInfo) {
    LogFormat("vkCreateSwapchainKHR result=%d %ux%u format=%u images=%u",
              static_cast<int>(result),
              static_cast<unsigned>(pCreateInfo->imageExtent.width),
              static_cast<unsigned>(pCreateInfo->imageExtent.height),
              static_cast<unsigned>(pCreateInfo->imageFormat),
              static_cast<unsigned>(pCreateInfo->minImageCount));
  } else {
    LogFormat("vkCreateSwapchainKHR result=%d", static_cast<int>(result));
  }
  return result;
}

extern "C" VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(
    VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo *pCreateInfos,
    const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines) {
  HelperInitialize();
  auto fn = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(
      GetRealDeviceProc(device, "vkCreateGraphicsPipelines"));
  const VkResult result =
      fn ? fn(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator,
              pPipelines)
         : VK_ERROR_INITIALIZATION_FAILED;
  LogFormat("vkCreateGraphicsPipelines result=%d count=%u",
            static_cast<int>(result),
            static_cast<unsigned>(createInfoCount));
  return result;
}

extern "C" VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(
    VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
    const VkComputePipelineCreateInfo *pCreateInfos,
    const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines) {
  HelperInitialize();
  auto fn = reinterpret_cast<PFN_vkCreateComputePipelines>(
      GetRealDeviceProc(device, "vkCreateComputePipelines"));
  const VkResult result =
      fn ? fn(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator,
              pPipelines)
         : VK_ERROR_INITIALIZATION_FAILED;
  LogFormat("vkCreateComputePipelines result=%d count=%u",
            static_cast<int>(result),
            static_cast<unsigned>(createInfoCount));
  return result;
}

extern "C" VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(
    VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule) {
  HelperInitialize();
  auto fn = reinterpret_cast<PFN_vkCreateShaderModule>(
      GetRealDeviceProc(device, "vkCreateShaderModule"));
  const VkResult result =
      fn ? fn(device, pCreateInfo, pAllocator, pShaderModule)
         : VK_ERROR_INITIALIZATION_FAILED;
  if (result == VK_SUCCESS && pCreateInfo && pCreateInfo->pCode &&
      pCreateInfo->codeSize > 0) {
    HelperSaveShaderIR(pCreateInfo->pCode, pCreateInfo->codeSize);
  }
  return result;
}

extern "C" VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(
    VkDevice device, VkShaderModule shaderModule,
    const VkAllocationCallbacks *pAllocator) {
  auto fn = reinterpret_cast<PFN_vkDestroyShaderModule>(
      GetRealDeviceProc(device, "vkDestroyShaderModule"));
  if (fn) {
    fn(device, shaderModule, pAllocator);
  }
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    g_selfModule = instance;
    DisableThreadLibraryCalls(instance);
  }
  return TRUE;
}
