#include "offsets.h"
#include "memory.h"

void Offsets::Init(uintptr_t moduleBase, HANDLE hProcess) {
    dwLocalPlayerPawn   = 0x1B0A3A0;
    dwEntityList        = 0x1E0E7D0;
    dwViewMatrix        = 0x1EA3B60;

    m_iHealth           = 0x334;
    m_iTeamNum          = 0x3C3;
    m_vecOrigin         = 0x12CC;
    m_aimPunchAngle     = 0x1784;
    m_pGameSceneNode    = 0x320;
    m_modelState        = 0x170;
    m_boneArray         = 0x80;
    m_hObserverTarget   = 0x34B0;
    v_angle             = 0x23B0;
}
