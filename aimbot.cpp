#include <windows.h>
#include "aimbot.h"
#include "memory.h"
#include "offsets.h"
#include <user32.h>
#include <cmath>

Vector3 CalcAngle(const Vector3& src, const Vector3& dst) {
    Vector3 angle;
    float deltaX = dst.x - src.x;
    float deltaY = dst.y - src.y;
    float deltaZ = dst.z - src.z;
    float hyp = sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ);
    angle.x = asinf(deltaZ / hyp) * (180.0f / M_PI);
    angle.y = atan2f(deltaY, deltaX) * (180.0f / M_PI);
    angle.z = 0;
    return angle;
}

Vector3 GetBonePos(HANDLE hProcess, uintptr_t player, int bone) {
    uintptr_t boneMatrix = Memory::Read<uintptr_t>(hProcess, player + Offsets::m_hBoneMatrix);
    Vector3 bonePos;
    bonePos.x = Memory::Read<float>(hProcess, boneMatrix + (0x30 * bone) + 0x0C);
    bonePos.y = Memory::Read<float>(hProcess, boneMatrix + (0x30 * bone) + 0x1C);
    bonePos.z = Memory::Read<float>(hProcess, boneMatrix + (0x30 * bone) + 0x2C);
    return bonePos;
}

bool IsVisible(HANDLE hProcess, uintptr_t player) {
    // Simple check, implement real vischeck if needed
    return true;
}

void UpdateAimbot(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer) {
    if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) return; // Hold right mouse

    Vector3 localPos = Memory::Read<Vector3>(hProcess, localPlayer + Offsets::m_vecOrigin);
    Vector3 localView = Memory::Read<Vector3>(hProcess, localPlayer + Offsets::clientState_ViewAngles); // Need clientState offset

    uintptr_t entityList = Memory::Read<uintptr_t>(hProcess, moduleBase + Offsets::dwEntityList);

    float closestDist = Config::aimbotFov;
    Vector3 targetAngle;

    for (int i = 1; i < 64; ++i) {
        uintptr_t player = Memory::Read<uintptr_t>(hProcess, entityList + i * 0x10);
        if (player == 0 || player == localPlayer) continue;

        int health = Memory::Read<int>(hProcess, player + Offsets::m_iHealth);
        if (health < 1 || health > 100) continue;

        int team = Memory::Read<int>(hProcess, player + Offsets::m_iTeamNum);
        if (team == Memory::Read<int>(hProcess, localPlayer + Offsets::m_iTeamNum)) continue;

        if (!IsVisible(hProcess, player)) continue;

        Vector3 bonePos = GetBonePos(hProcess, player, Config::aimbotBone);

        Vector3 calcAng = CalcAngle(localPos, bonePos);
        float dist = sqrt(pow(calcAng.x - localView.x, 2) + pow(calcAng.y - localView.y, 2));

        if (dist < closestDist) {
            closestDist = dist;
            targetAngle = calcAng;
        }
    }

    if (closestDist < Config::aimbotFov) {
        Vector3 delta = targetAngle - localView;
        delta.x *= Config::aimbotSmooth;
        delta.y *= Config::aimbotSmooth;
        // To set angles, but external: Simulate mouse movement
        int dx = static_cast<int>(delta.y * 2); // Adjust multiplier
        int dy = static_cast<int>(-delta.x * 2); // Y inverted?
        mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
    }
}
