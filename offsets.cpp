// offsets.cpp
#include "offsets.h"
#include "memory.h"

uintptr_t Offsets::dwLocalPlayerPawn = 0;
uintptr_t Offsets::dwEntityList = 0;
uintptr_t Offsets::dwViewMatrix = 0;
uintptr_t Offsets::m_iHealth = 0x334;
uintptr_t Offsets::m_iTeamNum = 0x3C3;
uintptr_t Offsets::m_vecOrigin = 0x12CC;
uintptr_t Offsets::m_aimPunchAngle = 0x1784;
uintptr_t Offsets::m_pGameSceneNode = 0x320;
uintptr_t Offsets::m_modelState = 0x170;
uintptr_t Offsets::m_boneArray = 0x80;
uintptr_t Offsets::v_angle = 0x23B0;

std::string Offsets::sigLocalPlayerPawn = "\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\x48\x39\x88";
std::string Offsets::maskLocalPlayerPawn = "xxx????xxxxxxxx";
std::string Offsets::sigEntityList = "\x48\x8B\x0D\x00\x00\x00\x00\x48\x89\x7C\x24\x00\x8B\xFA\xC1";
std::string Offsets::maskEntityList = "xxx????xxxx?xxx";
std::string Offsets::sigViewMatrix = "\x48\x8D\x0D\x00\x00\x00\x00\x48\xC7\x44\x24\x00\x00\x00\x00\x00\x48\x8B\x10";
std::string Offsets::maskViewMatrix = "xxx????xxxx?xxxxxxxxx";

void Offsets::Init(uintptr_t moduleBase, HANDLE hProcess) {
    uintptr_t addr = Memory::FindSignature(hProcess, moduleBase, 0x8000000, sigLocalPlayerPawn, maskLocalPlayerPawn);
    if (addr) {
        dwLocalPlayerPawn = Memory::Read<int32_t>(hProcess, addr + 3) + addr + 7 - moduleBase;
    }

    addr = Memory::FindSignature(hProcess, moduleBase, 0x8000000, sigEntityList, maskEntityList);
    if (addr) {
        dwEntityList = Memory::Read<int32_t>(hProcess, addr + 3) + addr + 7 - moduleBase;
    }

    addr = Memory::FindSignature(hProcess, moduleBase, 0x8000000, sigViewMatrix, maskViewMatrix);
    if (addr) {
        dwViewMatrix = Memory::Read<int32_t>(hProcess, addr + 3) + addr + 7 - moduleBase;
    }
}
