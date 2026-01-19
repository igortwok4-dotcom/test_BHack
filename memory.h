// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <windows.h>
#include <string>

namespace Memory {
    template <typename T>
    T Read(HANDLE hProcess, uintptr_t address);

    uintptr_t FindSignature(HANDLE hProcess, uintptr_t start, uintptr_t size, const std::string& sig, const std::string& mask);
}

uintptr_t GetModuleBaseAddress(DWORD pid, const std::string& moduleName);

#endif
