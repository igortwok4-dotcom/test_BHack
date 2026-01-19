#ifndef OVERLAY_H
#define OVERLAY_H

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

extern HWND g_overlayWindow;
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;

bool InitOverlay(HWND targetWindow);
void CleanupOverlay();

#endif // OVERLAY_H
