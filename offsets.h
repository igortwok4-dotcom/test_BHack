// offsets.h
#ifndef OFFSETS_H
#define OFFSETS_H

#include <windows.h>

// Structures
struct Vector3 {
    float x, y, z;
};

struct Player {
    Vector3 position;
    int health;
    int team;
    Vector3 viewAngles;
    // Add more as needed
};

// Offsets (hardcoded for simplicity, use sigscan in Init)
namespace Offsets {
    extern uintptr_t dwLocalPlayer;
    extern uintptr_t dwEntityList;
    extern uintptr_t m_iHealth;
    extern uintptr_t m_iTeamNum;
    extern uintptr_t m_vecOrigin;
    extern uintptr_t m_aimPunchAngle;
    extern uintptr_t m_hBoneMatrix;
    // etc.

    void Init(uintptr_t moduleBase, HANDLE hProcess);
}

#endif
