// main.cpp
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <cmath>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "memory.h"
#include "offsets.h"
#include "esp.h"
#include "aimbot.h"
#include "rcs.h"
#include "overlay.h"

#define OBF_STR(str) []() { constexpr char encrypted[] = { str }; std::string decrypted; for (char c : encrypted) { if (c == 0) break; decrypted += c ^ 0xAA; } return decrypted; }()
#define RAND_SLEEP(min, max) std::this_thread::sleep_for(std::chrono::milliseconds(min + rand() % (max - min + 1)))

namespace Config {
    bool espEnabled = true;
    bool aimbotEnabled = true;
    bool rcsEnabled = true;
    float aimbotFov = 5.0f;
    float aimbotSmooth = 0.2f;
    int aimbotBone = 8;
    float rcsStrength = 1.0f;
    bool showMenu = true;
}

bool AdvancedAntiDebug() {
    if (IsDebuggerPresent()) return true;
    CONTEXT ctx = {};
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    if (GetThreadContext(GetCurrentThread(), &ctx)) {
        if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3) return true;
    }
    PEB* peb = (PEB*)__readfsdword(0x30);
    if (peb->BeingDebugged) return true;
    return false;
}

void AdvancedMasquerade() {
    std::string fakeName = OBF_STR("\xC3\xE8\xE9\xE4\xE1\xE0\xE7\x2E\x65\x78\x65");
    SetConsoleTitleA(fakeName.c_str());
}

DWORD GetProcessId(const std::string& procName) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);
        if (Process32First(snapshot, &entry)) {
            do {
                if (std::string(entry.szExeFile) == procName) {
                    pid = entry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }
    return pid;
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    AdvancedMasquerade();

    if (AdvancedAntiDebug()) {
        exit(1);
    }

    std::string encProcName = OBF_STR("\x87\x93\x92\x2E\x65\x78\x65");
    DWORD pid = GetProcessId(encProcName);
    if (pid == 0) return 1;

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) return 1;

    uintptr_t moduleBase = GetModuleBaseAddress(pid, encProcName);
    if (moduleBase == 0) { CloseHandle(hProcess); return 1; }

    Offsets::Init(moduleBase, hProcess);

    HWND gameWindow = FindWindowA(NULL, "Counter-Strike 2");
    if (!InitOverlay(gameWindow)) { CloseHandle(hProcess); return 1; }

    std::cout << "Cheat running. Press INSERT to toggle menu, END to exit." << std::endl;

    while (!GetAsyncKeyState(VK_END) & 1) {
        RAND_SLEEP(5, 20);

        if (GetAsyncKeyState(VK_INSERT) & 1) Config::showMenu = !Config::showMenu;

        uintptr_t localPlayer = Memory::Read<uintptr_t>(hProcess, moduleBase + Offsets::dwLocalPlayerPawn);
        if (localPlayer == 0) continue;

        if (Config::aimbotEnabled) UpdateAimbot(hProcess, moduleBase, localPlayer);
        if (Config::rcsEnabled) UpdateRCS(hProcess, moduleBase, localPlayer);
    }

    CleanupOverlay();
    CloseHandle(hProcess);
    return 0;
}
}
