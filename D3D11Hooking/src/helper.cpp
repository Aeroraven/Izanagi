#include <d3d11.h>
#include <windows.h>
#include <shlobj.h>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <filesystem>
#include <mutex>
#include <new>
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
  swprintf_s(stamp, L"%04u%02u%02u-%02u%02u%02u-d3d11hook", st.wYear, st.wMonth,
             st.wDay, st.wHour, st.wMinute, st.wSecond);

  std::filesystem::path base(docs);
  base /= kRootFolderName;
  std::error_code ec;
  std::filesystem::create_directories(base, ec);
  g_rootDir = base / stamp;
  g_logDir = g_rootDir / L"log";
  std::filesystem::create_directories(g_logDir, ec);
  g_logFile = g_logDir / L"d3d11.log";
  g_ready.store(!g_rootDir.empty());
  if (g_ready.load()) {
    AppendLogLine("D3D11Hooking: session started");
  }
  return TRUE;
}

void EnsureInitialized() {
  InitOnceExecuteOnce(&g_initOnce, InitOnceProc, nullptr, nullptr);
}

void SaveShader(const void *data, size_t size, const wchar_t *ext) {
  if (!data || size == 0 || !ext) {
    return;
  }
  EnsureInitialized();
  if (!g_ready.load()) {
    return;
  }
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
  LogFormat("D3D11Hooking: saved shader %ls (%zu bytes)", filename.c_str(),
            size);
}

struct __declspec(uuid("EAEBE6BB-7C90-4D4D-8F80-1C3AF02354AE"))
    ID3D11DeviceProxyTag : IUnknown {};

class DeviceProxy final : public ID3D11Device {
public:
  explicit DeviceProxy(ID3D11Device *inner) : m_inner(inner) {}

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void **ppvObject) override {
    if (!ppvObject) {
      return E_POINTER;
    }
    if (riid == __uuidof(IUnknown) || riid == __uuidof(ID3D11Device) ||
        riid == __uuidof(ID3D11DeviceProxyTag)) {
      *ppvObject = static_cast<ID3D11Device *>(this);
      AddRef();
      return S_OK;
    }
    return m_inner ? m_inner->QueryInterface(riid, ppvObject) : E_NOINTERFACE;
  }

  ULONG STDMETHODCALLTYPE AddRef() override {
    const ULONG refs = ++m_refCount;
    if (m_inner) {
      m_inner->AddRef();
    }
    return refs;
  }

  ULONG STDMETHODCALLTYPE Release() override {
    const ULONG refs = --m_refCount;
    if (m_inner) {
      m_inner->Release();
    }
    if (refs == 0) {
      delete this;
    }
    return refs;
  }

  HRESULT STDMETHODCALLTYPE CreateBuffer(const D3D11_BUFFER_DESC *pDesc,
                                         const D3D11_SUBRESOURCE_DATA *pData,
                                         ID3D11Buffer **ppBuffer) override {
    return m_inner->CreateBuffer(pDesc, pData, ppBuffer);
  }

  HRESULT STDMETHODCALLTYPE CreateTexture1D(
      const D3D11_TEXTURE1D_DESC *pDesc,
      const D3D11_SUBRESOURCE_DATA *pInitialData,
      ID3D11Texture1D **ppTexture1D) override {
    return m_inner->CreateTexture1D(pDesc, pInitialData, ppTexture1D);
  }

  HRESULT STDMETHODCALLTYPE CreateTexture2D(
      const D3D11_TEXTURE2D_DESC *pDesc,
      const D3D11_SUBRESOURCE_DATA *pInitialData,
      ID3D11Texture2D **ppTexture2D) override {
    return m_inner->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
  }

  HRESULT STDMETHODCALLTYPE CreateTexture3D(
      const D3D11_TEXTURE3D_DESC *pDesc,
      const D3D11_SUBRESOURCE_DATA *pInitialData,
      ID3D11Texture3D **ppTexture3D) override {
    return m_inner->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
  }

  HRESULT STDMETHODCALLTYPE CreateShaderResourceView(
      ID3D11Resource *pResource,
      const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
      ID3D11ShaderResourceView **ppSRView) override {
    return m_inner->CreateShaderResourceView(pResource, pDesc, ppSRView);
  }

  HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView(
      ID3D11Resource *pResource,
      const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
      ID3D11UnorderedAccessView **ppUAView) override {
    return m_inner->CreateUnorderedAccessView(pResource, pDesc, ppUAView);
  }

  HRESULT STDMETHODCALLTYPE CreateRenderTargetView(
      ID3D11Resource *pResource,
      const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
      ID3D11RenderTargetView **ppRTView) override {
    return m_inner->CreateRenderTargetView(pResource, pDesc, ppRTView);
  }

  HRESULT STDMETHODCALLTYPE CreateDepthStencilView(
      ID3D11Resource *pResource,
      const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
      ID3D11DepthStencilView **ppDepthStencilView) override {
    return m_inner->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
  }

  HRESULT STDMETHODCALLTYPE CreateInputLayout(
      const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
      UINT NumElements, const void *pShaderBytecodeWithInputSignature,
      SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout) override {
    return m_inner->CreateInputLayout(pInputElementDescs, NumElements,
                                      pShaderBytecodeWithInputSignature,
                                      BytecodeLength, ppInputLayout);
  }

  HRESULT STDMETHODCALLTYPE CreateVertexShader(const void *pShaderBytecode,
                                               SIZE_T BytecodeLength,
                                               ID3D11ClassLinkage *pClassLinkage,
                                               ID3D11VertexShader **ppVertexShader) override {
    SaveShader(pShaderBytecode, BytecodeLength, L"vs");
    return m_inner->CreateVertexShader(pShaderBytecode, BytecodeLength,
                                       pClassLinkage, ppVertexShader);
  }

  HRESULT STDMETHODCALLTYPE CreateGeometryShader(
      const void *pShaderBytecode, SIZE_T BytecodeLength,
      ID3D11ClassLinkage *pClassLinkage,
      ID3D11GeometryShader **ppGeometryShader) override {
    SaveShader(pShaderBytecode, BytecodeLength, L"gs");
    return m_inner->CreateGeometryShader(pShaderBytecode, BytecodeLength,
                                         pClassLinkage, ppGeometryShader);
  }

  HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput(
      const void *pShaderBytecode, SIZE_T BytecodeLength,
      const D3D11_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries,
      const UINT *pBufferStrides, UINT NumStrides, UINT RasterizedStream,
      ID3D11ClassLinkage *pClassLinkage,
      ID3D11GeometryShader **ppGeometryShader) override {
    SaveShader(pShaderBytecode, BytecodeLength, L"gs");
    return m_inner->CreateGeometryShaderWithStreamOutput(
        pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries,
        pBufferStrides, NumStrides, RasterizedStream, pClassLinkage,
        ppGeometryShader);
  }

  HRESULT STDMETHODCALLTYPE CreatePixelShader(const void *pShaderBytecode,
                                              SIZE_T BytecodeLength,
                                              ID3D11ClassLinkage *pClassLinkage,
                                              ID3D11PixelShader **ppPixelShader) override {
    SaveShader(pShaderBytecode, BytecodeLength, L"ps");
    return m_inner->CreatePixelShader(pShaderBytecode, BytecodeLength,
                                      pClassLinkage, ppPixelShader);
  }

  HRESULT STDMETHODCALLTYPE CreateHullShader(const void *pShaderBytecode,
                                             SIZE_T BytecodeLength,
                                             ID3D11ClassLinkage *pClassLinkage,
                                             ID3D11HullShader **ppHullShader) override {
    SaveShader(pShaderBytecode, BytecodeLength, L"hs");
    return m_inner->CreateHullShader(pShaderBytecode, BytecodeLength,
                                     pClassLinkage, ppHullShader);
  }

  HRESULT STDMETHODCALLTYPE CreateDomainShader(const void *pShaderBytecode,
                                               SIZE_T BytecodeLength,
                                               ID3D11ClassLinkage *pClassLinkage,
                                               ID3D11DomainShader **ppDomainShader) override {
    SaveShader(pShaderBytecode, BytecodeLength, L"ds");
    return m_inner->CreateDomainShader(pShaderBytecode, BytecodeLength,
                                       pClassLinkage, ppDomainShader);
  }

  HRESULT STDMETHODCALLTYPE CreateComputeShader(const void *pShaderBytecode,
                                                SIZE_T BytecodeLength,
                                                ID3D11ClassLinkage *pClassLinkage,
                                                ID3D11ComputeShader **ppComputeShader) override {
    SaveShader(pShaderBytecode, BytecodeLength, L"cs");
    return m_inner->CreateComputeShader(pShaderBytecode, BytecodeLength,
                                        pClassLinkage, ppComputeShader);
  }

  HRESULT STDMETHODCALLTYPE CreateClassLinkage(
      ID3D11ClassLinkage **ppLinkage) override {
    return m_inner->CreateClassLinkage(ppLinkage);
  }

  HRESULT STDMETHODCALLTYPE CreateBlendState(
      const D3D11_BLEND_DESC *pBlendStateDesc,
      ID3D11BlendState **ppBlendState) override {
    return m_inner->CreateBlendState(pBlendStateDesc, ppBlendState);
  }

  HRESULT STDMETHODCALLTYPE CreateDepthStencilState(
      const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
      ID3D11DepthStencilState **ppDepthStencilState) override {
    return m_inner->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
  }

  HRESULT STDMETHODCALLTYPE CreateRasterizerState(
      const D3D11_RASTERIZER_DESC *pRasterizerDesc,
      ID3D11RasterizerState **ppRasterizerState) override {
    return m_inner->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
  }

  HRESULT STDMETHODCALLTYPE CreateSamplerState(
      const D3D11_SAMPLER_DESC *pSamplerDesc,
      ID3D11SamplerState **ppSamplerState) override {
    return m_inner->CreateSamplerState(pSamplerDesc, ppSamplerState);
  }

  HRESULT STDMETHODCALLTYPE CreateQuery(const D3D11_QUERY_DESC *pQueryDesc,
                                        ID3D11Query **ppQuery) override {
    return m_inner->CreateQuery(pQueryDesc, ppQuery);
  }

  HRESULT STDMETHODCALLTYPE CreatePredicate(
      const D3D11_QUERY_DESC *pPredicateDesc,
      ID3D11Predicate **ppPredicate) override {
    return m_inner->CreatePredicate(pPredicateDesc, ppPredicate);
  }

  HRESULT STDMETHODCALLTYPE CreateCounter(
      const D3D11_COUNTER_DESC *pCounterDesc,
      ID3D11Counter **ppCounter) override {
    return m_inner->CreateCounter(pCounterDesc, ppCounter);
  }

  HRESULT STDMETHODCALLTYPE CreateDeferredContext(UINT ContextFlags,
                                                  ID3D11DeviceContext **ppDeferredContext) override {
    return m_inner->CreateDeferredContext(ContextFlags, ppDeferredContext);
  }

  HRESULT STDMETHODCALLTYPE OpenSharedResource(HANDLE hResource, REFIID ReturnedInterface,
                                               void **ppResource) override {
    return m_inner->OpenSharedResource(hResource, ReturnedInterface, ppResource);
  }

  HRESULT STDMETHODCALLTYPE CheckFormatSupport(DXGI_FORMAT Format,
                                               UINT *pFormatSupport) override {
    return m_inner->CheckFormatSupport(Format, pFormatSupport);
  }

  HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels(
      DXGI_FORMAT Format, UINT SampleCount,
      UINT *pNumQualityLevels) override {
    return m_inner->CheckMultisampleQualityLevels(Format, SampleCount,
                                                  pNumQualityLevels);
  }

  void STDMETHODCALLTYPE CheckCounterInfo(D3D11_COUNTER_INFO *pCounterInfo) override {
    m_inner->CheckCounterInfo(pCounterInfo);
  }

  HRESULT STDMETHODCALLTYPE CheckCounter(const D3D11_COUNTER_DESC *pDesc,
                                         D3D11_COUNTER_TYPE *pType,
                                         UINT *pActiveCounters, LPSTR szName,
                                         UINT *pNameLength, LPSTR szUnits,
                                         UINT *pUnitsLength, LPSTR szDescription,
                                         UINT *pDescriptionLength) override {
    return m_inner->CheckCounter(pDesc, pType, pActiveCounters, szName,
                                 pNameLength, szUnits, pUnitsLength,
                                 szDescription, pDescriptionLength);
  }

  HRESULT STDMETHODCALLTYPE CheckFeatureSupport(
      D3D11_FEATURE Feature, void *pFeatureSupportData,
      UINT FeatureSupportDataSize) override {
    return m_inner->CheckFeatureSupport(Feature, pFeatureSupportData,
                                        FeatureSupportDataSize);
  }

  HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID guid, UINT *pDataSize,
                                           void *pData) override {
    return m_inner->GetPrivateData(guid, pDataSize, pData);
  }

  HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID guid, UINT DataSize,
                                           const void *pData) override {
    return m_inner->SetPrivateData(guid, DataSize, pData);
  }

  HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID guid,
                                                    const IUnknown *pData) override {
    return m_inner->SetPrivateDataInterface(guid, pData);
  }

  D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel() override {
    return m_inner->GetFeatureLevel();
  }

  UINT STDMETHODCALLTYPE GetCreationFlags() override {
    return m_inner->GetCreationFlags();
  }

  HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason() override {
    return m_inner->GetDeviceRemovedReason();
  }

  void STDMETHODCALLTYPE GetImmediateContext(
      ID3D11DeviceContext **ppImmediateContext) override {
    m_inner->GetImmediateContext(ppImmediateContext);
  }

  HRESULT STDMETHODCALLTYPE SetExceptionMode(UINT RaiseFlags) override {
    return m_inner->SetExceptionMode(RaiseFlags);
  }

  UINT STDMETHODCALLTYPE GetExceptionMode() override {
    return m_inner->GetExceptionMode();
  }

private:
  std::atomic<ULONG> m_refCount{1};
  ID3D11Device *m_inner = nullptr;
};
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

extern "C" __declspec(dllexport) void __stdcall HelperWrapDevice(
    ID3D11Device **device) {
  if (!device || !*device) {
    return;
  }
  ID3D11DeviceProxyTag *tag = nullptr;
  if (SUCCEEDED((*device)->QueryInterface(__uuidof(ID3D11DeviceProxyTag),
                                          reinterpret_cast<void **>(&tag)))) {
    if (tag) {
      tag->Release();
    }
    return;
  }
  ID3D11Device *inner = *device;
  DeviceProxy *proxy = new (std::nothrow) DeviceProxy(inner);
  if (proxy) {
    *device = proxy;
  }
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(instance);
  }
  return TRUE;
}
