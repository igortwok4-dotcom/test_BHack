#ifndef RCS_H
#define RCS_H

#include <windows.h>
#include <cstdint>

struct Vector3 {
    float x, y, z;

    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }
};

namespace Config {
    extern bool rcsEnabled;
    extern float rcsStrength;
}

void UpdateRCS(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer);

#endif // RCS_H
