// offsets.cpp (add if needed, but for now inline in h)
#include "offsets.h"
#include "memory.h"

uintptr_t Offsets::dwLocalPlayer = 0xDEADBEEF; // Replace with real or sig
uintptr_t Offsets::dwEntityList = 0xBEEFDEAD;
uintptr_t Offsets::m_iHealth = 0x320;
uintptr_t Offsets::m_iTeamNum = 0xF4;
uintptr_t Offsets::m_vecOrigin = 0x138;
uintptr_t Offsets::m_aimPunchAngle = 0x3040;
uintptr_t Offsets::m_hBoneMatrix = 0x26A8;

void Offsets::Init(uintptr_t moduleBase, HANDLE hProcess) {
    // Example sigscan for dwLocalPlayer
    std::string sig = "\xA1\x00\x00\x00\x00\x8B\x00\x85\xC0\x74\x0A"; // Example sig, replace with real CS2 sig
    std::string mask = "x????x?xxx?";
    dwLocalPlayer = Memory::FindSignature(hProcess, moduleBase, 0x10000000, sig, mask) - moduleBase + 1; // Adjust
    // Do for others
    // Note: Real sigs needed from Cheat Engine or reverse engineering
}
