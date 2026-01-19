// main.cpp
// This is a simple external cheat for CS2 for testing anti-cheat only.
// Compile with MinGW-w64: x86_64-w64-mingw32-g++ -o cheat.exe main.cpp memory.cpp esp.cpp aimbot.cpp rcs.cpp -mwindows -lpsapi -luser32 -static
// Run on Windows where CS2 is running.

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <cmath>
#include "memory.h"
#include "offsets.h"
#include "esp.h"
#include "aimbot.h"
#include "rcs.h"

// Configurable settings
namespace Config {
    bool espEnabled = true;
    bool aimbotEnabled = true;
    bool rcsEnabled = true;
    float aimbotFov = 5.0f; // Degrees
    float aimbotSmooth = 0.2f; // 0.0 = instant, 1.0 = very slow
    int aimbotBone = 8; // 8 = head
    float rcsStrength = 1.0f; // 1.0 = full compensation
    std::string processName = "cs2.exe"; // Target process
}

// Anti-debug basics
bool IsDebuggerPresentSimple() {
    return IsDebuggerPresent();
}

void RandomSleep(int minMs, int maxMs) {
    int sleepTime = minMs + rand() % (maxMs - minMs + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
}

// XOR encrypt/decrypt simple
std::string XorEncryptDecrypt(const std::string& str, char key = 0xAA) {
    std::string result = str;
    for (char& c : result) {
        c ^= key;
    }
    return result;
}

// Masquerade: Change window title
void MasqueradeProcess() {
    std::string fakeTitle = XorEncryptDecrypt("\xC3\xE8\xE9\xE4\xE1\xE0\xE7\x2E\x65\x78\x65"); // Encrypted "explorer.exe"
    SetConsoleTitleA(fakeTitle.c_str());
}

// Find process ID by name
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

// Main loop
int main() {
    srand(static_cast<unsigned>(time(0)));
    MasqueradeProcess();

    if (IsDebuggerPresentSimple()) {
        std::cout << "Debugger detected! Exiting." << std::endl;
        return 1;
    }

    std::string encProcName = XorEncryptDecrypt("\x87\x93\x92\x2E\x65\x78\x65"); // Encrypted "cs2.exe"
    DWORD pid = GetProcessId(encProcName);
    if (pid == 0) {
        std::cout << "CS2 not found!" << std::endl;
        return 1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        std::cout << "Failed to open process!" << std::endl;
        return 1;
    }

    uintptr_t moduleBase = GetModuleBaseAddress(pid, encProcName);
    if (moduleBase == 0) {
        std::cout << "Failed to find module base!" << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    // Find dynamic offsets using signatures (simplified, add real sigs)
    Offsets::Init(moduleBase, hProcess);

    std::cout << "Cheat running. Press END to exit." << std::endl;

    while (!GetAsyncKeyState(VK_END) & 1) {
        RandomSleep(10, 30); // Randomize loop timing

        uintptr_t localPlayer = Memory::Read<uintptr_t>(hProcess, moduleBase + Offsets::dwLocalPlayer);
        if (localPlayer == 0) continue;

        if (Config::espEnabled) {
            DrawESP(hProcess, moduleBase, localPlayer);
        }

        if (Config::aimbotEnabled) {
            UpdateAimbot(hProcess, moduleBase, localPlayer);
        }

        if (Config::rcsEnabled) {
            UpdateRCS(hProcess, moduleBase, localPlayer);
        }
    }

    CloseHandle(hProcess);
    return 0;
}
