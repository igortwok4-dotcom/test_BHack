// offsets.h
#ifndef OFFSETS_H
#define OFFSETS_H

#include <windows.h>

struct Vector3 {
    float x, y, z;
};

namespace Offsets {
    extern uintptr_t dwLocalPlayerPawn;
    extern uintptr_t dwEntityList;
    extern uintptr_t dwViewMatrix;
    extern uintptr_t m_iHealth;
    extern uintptr_t m_iTeamNum;
    extern uintptr_t m_vecOrigin;
    extern uintptr_t m_aimPunchAngle;
    extern uintptr_t m_pGameSceneNode;
    extern uintptr_t m_modelState;
    extern uintptr_t m_boneArray;
    extern uintptr_t v_angle;

    extern std::string sigLocalPlayerPawn;
    extern std::string maskLocalPlayerPawn;
    extern std::string sigEntityList;
    extern std::string maskEntityList;
    extern std::string sigViewMatrix;
    extern std::string maskViewMatrix;

    void Init(uintptr_t moduleBase, HANDLE hProcess);
}

#endif
