// overlay.cpp
#include "overlay.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "esp.h"

HWND g_overlayWindow = NULL;
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK) return false;

    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
    return true;
}

void RenderImGui(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (Config::showMenu) {
        ImGui::Begin("CS2 Cheat Menu");
        ImGui::Checkbox("ESP", &Config::espEnabled);
        ImGui::Checkbox("Aimbot", &Config::aimbotEnabled);
        ImGui::SliderFloat("Aimbot FOV", &Config::aimbotFov, 1.0f, 30.0f);
        ImGui::SliderFloat("Aimbot Smooth", &Config::aimbotSmooth, 0.1f, 1.0f);
        ImGui::Checkbox("RCS", &Config::rcsEnabled);
        ImGui::SliderFloat("RCS Strength", &Config::rcsStrength, 0.5f, 2.0f);
        ImGui::End();
    }

    if (Config::espEnabled) {
        DrawESP(hProcess, moduleBase, localPlayer);
    }

    ImGui::Render();
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    switch (msg) {
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool InitOverlay(HWND targetWindow) {
    RECT rect;
    GetWindowRect(targetWindow, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "Overlay", NULL };
    RegisterClassEx(&wc);

    g_overlayWindow = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, "Overlay", "Overlay", WS_POPUP, rect.left, rect.top, width, height, NULL, NULL, wc.hInstance, NULL);
    SetLayeredWindowAttributes(g_overlayWindow, RGB(0,0,0), 0, ULW_COLORKEY);
    ShowWindow(g_overlayWindow, SW_SHOW);

    if (!CreateDeviceD3D(g_overlayWindow)) return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(g_overlayWindow);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    std::thread renderThread([](HANDLE hProcess, uintptr_t moduleBase) {
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (::GetMessage(&msg, NULL, 0U, 0U)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            float clear_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
            uintptr_t localPlayer = Memory::Read<uintptr_t>(hProcess, moduleBase + Offsets::dwLocalPlayerPawn);
            RenderImGui(hProcess, moduleBase, localPlayer);
            g_pSwapChain->Present(1, 0);
        }
    }, hProcess, moduleBase);
    renderThread.detach();

    return true;
}

void CleanupOverlay() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    g_pSwapChain->Release();
    g_mainRenderTargetView->Release();
    g_pd3dDeviceContext->Release();
    g_pd3dDevice->Release();
    DestroyWindow(g_overlayWindow);
}
