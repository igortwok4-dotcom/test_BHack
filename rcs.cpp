#include <windows.h>
#include "rcs.h"
#include "memory.h"
#include "offsets.h"
#include <user32.h>

Vector3 previousPunch = {0,0,0};

void UpdateRCS(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer) {
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) return; // While shooting

    Vector3 punchAngle = Memory::Read<Vector3>(hProcess, localPlayer + Offsets::m_aimPunchAngle);
    Vector3 delta = punchAngle - previousPunch;
    previousPunch = punchAngle;

    // Compensate for specific weapons? Simple for all
    int dx = static_cast<int>(delta.y * 2.0f * Config::rcsStrength);
    int dy = static_cast<int>(delta.x * 2.0f * Config::rcsStrength);
    mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
}
