#include <windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <stdexcept>
#include <string>
#include <filesystem>

using Microsoft::WRL::ComPtr;

struct Vertex {
    float position[3];
    float color[3];
};

HWND g_hWnd = nullptr;
UINT g_width = 1280;
UINT g_height = 720;

ComPtr<ID3D11Device> g_device;
ComPtr<ID3D11DeviceContext> g_context;
ComPtr<IDXGISwapChain> g_swapChain;
ComPtr<ID3D11RenderTargetView> g_renderTargetView;
ComPtr<ID3D11VertexShader> g_vertexShader;
ComPtr<ID3D11PixelShader> g_pixelShader;
ComPtr<ID3D11InputLayout> g_inputLayout;
ComPtr<ID3D11Buffer> g_vertexBuffer;

inline void ThrowIfFailed(HRESULT hr, const char* where = "")
{
    if (FAILED(hr)) {
        char msg[256];
        if (where && where[0] != '\0') {
            sprintf_s(msg, "HRESULT 0x%08X at %s", static_cast<unsigned>(hr), where);
        } else {
            sprintf_s(msg, "HRESULT 0x%08X", static_cast<unsigned>(hr));
        }
        throw std::runtime_error(msg);
    }
}

#define HR(expr) ThrowIfFailed((expr), #expr)

std::filesystem::path GetExecutableDir()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    std::filesystem::path exePath(buffer);
    return exePath.remove_filename();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            DestroyWindow(hWnd);
            return 0;
        }
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DX11TriangleClass";
    RegisterClassEx(&wc);

    RECT windowRect = {0, 0, static_cast<LONG>(g_width), static_cast<LONG>(g_height)};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    g_hWnd = CreateWindow(wc.lpszClassName, L"DirectX 11 Triangle", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          windowRect.right - windowRect.left,
                          windowRect.bottom - windowRect.top,
                          nullptr, nullptr, hInstance, nullptr);
    ShowWindow(g_hWnd, nCmdShow);
}

void InitD3D()
{
    UINT deviceFlags = 0;
#if defined(_DEBUG)
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = g_width;
    swapChainDesc.BufferDesc.Height = g_height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = g_hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    HR(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &g_swapChain,
        &g_device,
        &featureLevel,
        &g_context));

    ComPtr<ID3D11Texture2D> backBuffer;
    HR(g_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
    HR(g_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &g_renderTargetView));

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(g_width);
    viewport.Height = static_cast<float>(g_height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    g_context->RSSetViewports(1, &viewport);
}

void LoadAssets()
{
    auto shaderPath = GetExecutableDir() / "shaders.hlsl";
    if (!std::filesystem::exists(shaderPath)) {
        MessageBoxW(nullptr, (L"Missing shader file: " + shaderPath.wstring()).c_str(), L"Error", MB_ICONERROR | MB_OK);
        throw std::runtime_error("shaders.hlsl missing");
    }

    ComPtr<ID3DBlob> vertexShaderBlob;
    ComPtr<ID3DBlob> pixelShaderBlob;
    ComPtr<ID3DBlob> errorBlob;
    UINT compileFlags = 0;
#if defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hrVS = D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShaderBlob, &errorBlob);
    if (FAILED(hrVS)) {
        std::string err = errorBlob ? std::string(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize()) : "unknown error";
        MessageBoxA(nullptr, err.c_str(), "HLSL VS compile failed", MB_ICONERROR | MB_OK);
        ThrowIfFailed(hrVS, "D3DCompileFromFile VS");
    }

    errorBlob.Reset();
    HRESULT hrPS = D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShaderBlob, &errorBlob);
    if (FAILED(hrPS)) {
        std::string err = errorBlob ? std::string(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize()) : "unknown error";
        MessageBoxA(nullptr, err.c_str(), "HLSL PS compile failed", MB_ICONERROR | MB_OK);
        ThrowIfFailed(hrPS, "D3DCompileFromFile PS");
    }

    HR(g_device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &g_vertexShader));
    HR(g_device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &g_pixelShader));

    D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    HR(g_device->CreateInputLayout(inputLayout, static_cast<UINT>(_countof(inputLayout)),
                                   vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(),
                                   &g_inputLayout));

    Vertex triangleVertices[] = {
        {{0.0f, 0.25f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.25f, -0.25f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.25f, -0.25f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    };

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(triangleVertices);
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = triangleVertices;

    HR(g_device->CreateBuffer(&vertexBufferDesc, &vertexData, &g_vertexBuffer));
}

void Render()
{
    const float clearColor[4] = {0.1f, 0.1f, 0.3f, 1.0f};
    g_context->OMSetRenderTargets(1, g_renderTargetView.GetAddressOf(), nullptr);
    g_context->ClearRenderTargetView(g_renderTargetView.Get(), clearColor);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    g_context->IASetInputLayout(g_inputLayout.Get());
    g_context->IASetVertexBuffers(0, 1, g_vertexBuffer.GetAddressOf(), &stride, &offset);
    g_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    g_context->VSSetShader(g_vertexShader.Get(), nullptr, 0);
    g_context->PSSetShader(g_pixelShader.Get(), nullptr, 0);

    g_context->Draw(3, 0);

    g_swapChain->Present(1, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    try {
        InitWindow(hInstance, nCmdShow);
        InitD3D();
        LoadAssets();

        MSG msg = {};
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                Render();
            }
        }
    } catch (const std::exception& ex) {
        MessageBoxA(nullptr, ex.what(), "Error", MB_ICONERROR | MB_OK);
    }

    return 0;
}