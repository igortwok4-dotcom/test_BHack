// memory.cpp
#include "memory.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <vector>

template <typename T>
T Memory::Read(HANDLE hProcess, uintptr_t address) {
    T value;
    ReadProcessMemory(hProcess, (LPCVOID)address, &value, sizeof(T), NULL);
    return value;
}

// Explicit instantiations (for common types)
template float Memory::Read<float>(HANDLE, uintptr_t);
template int Memory::Read<int>(HANDLE, uintptr_t);
template uintptr_t Memory::Read<uintptr_t>(HANDLE, uintptr_t);
// Add more if needed

uintptr_t Memory::FindSignature(HANDLE hProcess, uintptr_t start, uintptr_t size, const std::string& sig, const std::string& mask) {
    std::vector<byte> buffer(size);
    ReadProcessMemory(hProcess, (LPCVOID)start, buffer.data(), size, NULL);

    for (uintptr_t i = 0; i < size - sig.length(); ++i) {
        bool found = true;
        for (size_t j = 0; j < sig.length(); ++j) {
            if (mask[j] != '?' && sig[j] != buffer[i + j]) {
                found = false;
                break;
            }
        }
        if (found) return start + i;
    }
    return 0;
}

uintptr_t GetModuleBaseAddress(DWORD pid, const std::string& moduleName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (std::string(modEntry.szModule) == moduleName) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}
