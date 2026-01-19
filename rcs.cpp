#include "rcs.h"
#include "memory.h"
#include "offsets.h"
#include <winuser.h>

static Vector3 oldPunch = { 0.0f, 0.0f, 0.0f };

void UpdateRCS(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer)
{
    if (!Config::rcsEnabled)
        return;

    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
    {
        oldPunch = { 0.0f, 0.0f, 0.0f };
        return;
    }

    Vector3 punch = Memory::Read<Vector3>(hProcess, localPlayer + Offsets::m_aimPunchAngle);
    Vector3 delta = punch - oldPunch;
    oldPunch = punch;

    if (delta.x == 0.0f && delta.y == 0.0f)
        return;

    int dx = static_cast<int>(-delta.y * 2.0f * Config::rcsStrength);
    int dy = static_cast<int>(-delta.x * 2.0f * Config::rcsStrength);

    mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
}
