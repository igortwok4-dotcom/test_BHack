#ifndef RCS_H
#define RCS_H

#include <windows.h>
#include <cstdint>
#include <memory.h>


namespace Config {
    extern bool rcsEnabled;
    extern float rcsStrength;
}

void UpdateRCS(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer);

#endif // RCS_H
