#include "overlay.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "esp.h"
#include "aimbot.h"
#include "rcs.h"
#include "memory.h"
#include "offsets.h"

HWND g_overlayWindow = NULL;
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

extern HANDLE g_hProcess;
extern uintptr_t g_moduleBase;

bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
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

    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();

    return true;
}

extern namespace Config;

void RenderOverlay() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (Config::showMenu) {
        ImGui::Begin("CS2 Tool", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("CS2 Memory Visualization Tool");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Visualization")) {
            ImGui::Checkbox("Enable Overlay", &Config::espEnabled);
        }

        if (ImGui::CollapsingHeader("Input Simulation")) {
            ImGui::Checkbox("Enable Aimbot", &Config::aimbotEnabled);
            ImGui::SliderFloat("FOV", &Config::aimbotFov, 1.0f, 30.0f, "%.1f degrees");
            ImGui::SliderFloat("Smooth", &Config::aimbotSmooth, 0.01f, 1.0f, "%.2f");
            const char* bones[] = { "Head", "Neck", "Chest" };
            ImGui::Combo("Target Bone", &Config::aimbotBone, bones, IM_ARRAYSIZE(bones));
        }

        if (ImGui::CollapsingHeader("Recoil Compensation")) {
            ImGui::Checkbox("Enable RCS", &Config::rcsEnabled);
            ImGui::SliderFloat("Strength", &Config::rcsStrength, 0.5f, 2.0f, "%.2f");
        }

        ImGui::End();
    }

    uintptr_t localPlayer = Memory::Read<uintptr_t>(g_hProcess, g_moduleBase + Offsets::dwLocalPlayerPawn);
    if (localPlayer && Config::espEnabled) {
        DrawESP(g_hProcess, g_moduleBase, localPlayer);
    }

    ImGui::EndFrame();
    ImGui::Render();

    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool InitOverlay(HWND targetWindow) {
    RECT rect;
    GetWindowRect(targetWindow, &rect);

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL, L"OverlayWindow", NULL };
    RegisterClassEx(&wc);

    g_overlayWindow = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, L"OverlayWindow", L"Overlay", WS_POPUP,
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);

    if (!g_overlayWindow) return false;

    SetLayeredWindowAttributes(g_overlayWindow, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(g_overlayWindow, SW_SHOW);
    UpdateWindow(g_overlayWindow);

    if (!CreateDeviceD3D(g_overlayWindow)) {
        CleanupOverlay();
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_overlayWindow);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    std::thread([]() {
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));

        while (true) {
            if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                if (msg.message == WM_QUIT) break;
            }

            RenderOverlay();
        }
    }).detach();

    return true;
}

void CleanupOverlay() {
    if (g_pd3dDevice) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (g_mainRenderTargetView) g_mainRenderTargetView->Release();
        if (g_pSwapChain) g_pSwapChain->Release();
        if (g_pd3dDeviceContext) g_pd3dDeviceContext->Release();
        if (g_pd3dDevice) g_pd3dDevice->Release();
    }

    if (g_overlayWindow) {
        DestroyWindow(g_overlayWindow);
    }

    UnregisterClass(L"OverlayWindow", GetModuleHandle(NULL));
}
