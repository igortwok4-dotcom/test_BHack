#include "aimbot.h"
#include "memory.h"
#include "offsets.h"
#include <winuser.h>
#include <cmath>

Vector3 CalcAngle(const Vector3& src, const Vector3& dst) {
    Vector3 delta = { dst.x - src.x, dst.y - src.y, dst.z - src.z };
    float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);
    Vector3 angle;
    angle.x = asinf(delta.z / hyp) * (180.0f / 3.14159265358979323846f);
    angle.y = atan2f(delta.y, delta.x) * (180.0f / 3.14159265358979323846f);
    if (angle.y < 0) angle.y += 360.0f;
    angle.z = 0.0f;
    return angle;
}

Vector3 GetBonePos(HANDLE hProcess, uintptr_t player, int bone) {
    uintptr_t gameSceneNode = Memory::Read<uintptr_t>(hProcess, player + Offsets::m_pGameSceneNode);
    uintptr_t boneMatrix = Memory::Read<uintptr_t>(hProcess, gameSceneNode + Offsets::m_modelState + Offsets::m_boneArray);
    Vector3 bonePos;
    bonePos.x = Memory::Read<float>(hProcess, boneMatrix + 0x30 * bone + 0x0C);
    bonePos.y = Memory::Read<float>(hProcess, boneMatrix + 0x30 * bone + 0x1C);
    bonePos.z = Memory::Read<float>(hProcess, boneMatrix + 0x30 * bone + 0x2C);
    return bonePos;
}

bool IsVisible(HANDLE hProcess, uintptr_t player) {
    return true;
}

void UpdateAimbot(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer) {
    if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) return;

    Vector3 localPos = Memory::Read<Vector3>(hProcess, localPlayer + Offsets::m_vecOrigin);
    Vector3 localView = Memory::Read<Vector3>(hProcess, localPlayer + Offsets::v_angle);

    uintptr_t entityList = Memory::Read<uintptr_t>(hProcess, moduleBase + Offsets::dwEntityList);

    float closestDist = Config::aimbotFov;
    Vector3 targetAngle = {};

    for (int i = 1; i < 64; ++i) {
        uintptr_t listEntry = Memory::Read<uintptr_t>(hProcess, entityList + i * 0x78);
        if (listEntry == 0) continue;
        uintptr_t player = Memory::Read<uintptr_t>(hProcess, listEntry + 0x10);
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
        int dx = static_cast<int>(delta.y * 2.0f);
        int dy = static_cast<int>(delta.x * 2.0f);
        mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
    }
}
