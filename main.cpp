#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "memory.h"
#include "offsets.h"
#include "esp.h"
#include "aimbot.h"
#include "rcs.h"
#include "overlay.h"

namespace Config {
    bool espEnabled = true;
    bool aimbotEnabled = true;
    bool rcsEnabled = true;
    float aimbotFov = 5.0f;
    float aimbotSmooth = 0.2f;
    int aimbotBone = 6;
    float rcsStrength = 1.0f;
    bool showMenu = true;
}

bool AdvancedAntiDebug() {
    if (IsDebuggerPresent()) return true;
    PEB* peb = (PEB*)__readfsdword(0x30);
    if (peb->BeingDebugged) return true;
    return false;
}

DWORD GetProcessId(const std::string& procName) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);
        if (Process32First(snapshot, &entry)) {
            do {
                if (_stricmp(entry.szExeFile, procName.c_str()) == 0) {
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
    srand(static_cast<unsigned>(time(nullptr)));

    if (AdvancedAntiDebug()) {
        return 1;
    }

    std::string procName = "cs2.exe";
    DWORD pid = GetProcessId(procName);
    if (pid == 0) {
        std::cout << "CS2 process not found" << std::endl;
        return 1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProcess) {
        std::cout << "Failed to open process" << std::endl;
        return 1;
    }

    uintptr_t moduleBase = GetModuleBaseAddress(pid, procName);
    if (!moduleBase) {
        std::cout << "Failed to get module base address" << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    Offsets::Init(moduleBase, hProcess);

    HWND gameWnd = FindWindowA(nullptr, "Counter-Strike 2");
    if (!gameWnd) {
        std::cout << "CS2 window not found" << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    if (!InitOverlay(gameWnd)) {
        std::cout << "Failed to initialize overlay" << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    std::cout << "Tool running. Press INSERT to toggle menu, END to exit" << std::endl;

    while (!(GetAsyncKeyState(VK_END) & 1)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5 + rand() % 15));

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            Config::showMenu = !Config::showMenu;
        }

        uintptr_t localPlayer = Memory::Read<uintptr_t>(hProcess, moduleBase + Offsets::dwLocalPlayerPawn);
        if (!localPlayer) continue;

        if (Config::aimbotEnabled) UpdateAimbot(hProcess, moduleBase, localPlayer);
        if (Config::rcsEnabled) UpdateRCS(hProcess, moduleBase, localPlayer);
    }

    CleanupOverlay();
    CloseHandle(hProcess);
    return 0;
}
