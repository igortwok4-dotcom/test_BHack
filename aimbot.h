#ifndef AIMBOT_H
#define AIMBOT_H

#include <windows.h>
#include <cstdint>  // для uintptr_t
#include <memory.h>

namespace Config {
    extern bool aimbotEnabled;
    extern float aimbotFov;
    extern float aimbotSmooth;
    extern int aimbotBone;
}

// Основные функции aimbot'а
Vector3 CalcAngle(const Vector3& src, const Vector3& dst);
Vector3 GetBonePos(HANDLE hProcess, uintptr_t player, int bone);
bool IsVisible(HANDLE hProcess, uintptr_t player);
void UpdateAimbot(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer);

#endif // AIMBOT_H
