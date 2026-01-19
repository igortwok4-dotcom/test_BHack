#ifndef OFFSETS_H
#define OFFSETS_H

#include <windows.h>
#include <cstdint>
#include <memory.h>

struct Vector3 {
    float x, y, z;


namespace Offsets {
    inline uintptr_t dwLocalPlayerPawn   = 0x1B0A3A0;
    inline uintptr_t dwEntityList        = 0x1E0E7D0;
    inline uintptr_t dwViewMatrix        = 0x1EA3B60;

    inline uintptr_t m_iHealth           = 0x334;
    inline uintptr_t m_iTeamNum          = 0x3C3;
    inline uintptr_t m_vecOrigin         = 0x12CC;
    inline uintptr_t m_aimPunchAngle     = 0x1784;
    inline uintptr_t m_pGameSceneNode    = 0x320;
    inline uintptr_t m_modelState        = 0x170;
    inline uintptr_t m_boneArray         = 0x80;

    inline uintptr_t m_hObserverTarget   = 0x34B0;

    inline uintptr_t v_angle             = 0x23B0;

    void Init(uintptr_t moduleBase, HANDLE hProcess);
}

#endif // OFFSETS_H
