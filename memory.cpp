#include "memory.h"
#include <psapi.h>
#include <vector>

template <typename T>
T Memory::Read(HANDLE hProcess, uintptr_t address) {
    T value = {};
    if (hProcess && address) {
        ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), nullptr);
    }
    return value;
}

template <> float Memory::Read<float>(HANDLE hProcess, uintptr_t address) {
    float value = 0.0f;
    ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(float), nullptr);
    return value;
}

template <> int Memory::Read<int>(HANDLE hProcess, uintptr_t address) {
    int value = 0;
    ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(int), nullptr);
    return value;
}

template <> uint32_t Memory::Read<uint32_t>(HANDLE hProcess, uintptr_t address) {
    uint32_t value = 0;
    ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(uint32_t), nullptr);
    return value;
}

template <> uint64_t Memory::Read<uint64_t>(HANDLE hProcess, uintptr_t address) {
    uint64_t value = 0;
    ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(uint64_t), nullptr);
    return value;
}

template <> uintptr_t Memory::Read<uintptr_t>(HANDLE hProcess, uintptr_t address) {
    uintptr_t value = 0;
    ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(uintptr_t), nullptr);
    return value;
}

template <> Vector3 Memory::Read<Vector3>(HANDLE hProcess, uintptr_t address) {
    Vector3 value = {0.0f, 0.0f, 0.0f};
    ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(Vector3), nullptr);
    return value;
}

uintptr_t Memory::FindSignature(
    HANDLE hProcess,
    uintptr_t startAddress,
    size_t regionSize,
    const std::string& signature,
    const std::string& mask
) {
    std::vector<uint8_t> buffer(regionSize);
    if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(startAddress), buffer.data(), regionSize, nullptr)) {
        return 0;
    }

    for (size_t i = 0; i < regionSize - signature.length(); ++i) {
        bool found = true;
        for (size_t j = 0; j < signature.length(); ++j) {
            if (mask[j] != '?' && signature[j] != buffer[i + j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return startAddress + i;
        }
    }

    return 0;
}

uintptr_t Memory::GetModuleBaseAddress(DWORD processId, const std::string& moduleName) {
    uintptr_t moduleBase = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &moduleEntry)) {
        do {
            if (_stricmp(moduleEntry.szModule, moduleName.c_str()) == 0) {
                moduleBase = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
                break;
            }
        } while (Module32Next(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    return moduleBase;
}
