// rcs.cpp
#include "rcs.h"
#include "memory.h"
#include "offsets.h"
#include <winuser.h>

Vector3 previousPunch = {0,0,0};

void UpdateRCS(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer) {
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) return;

    Vector3 punchAngle = Memory::Read<Vector3>(hProcess, localPlayer + Offsets::m_aimPunchAngle);
    Vector3 delta = punchAngle - previousPunch;
    previousPunch = punchAngle;

    int dx = static_cast<int>(-delta.y * 2.0f * Config::rcsStrength);
    int dy = static_cast<int>(-delta.x * 2.0f * Config::rcsStrength);
    mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
}
