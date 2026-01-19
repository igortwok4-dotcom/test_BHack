// esp.cpp
#include "esp.h"
#include "memory.h"
#include "offsets.h"
#include <imgui.h>

struct Vector2 {
    float x, y;
};

bool WorldToScreen(const Vector3& pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight) {
    float clipX = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
    float clipY = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
    float clipZ = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
    float clipW = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

    if (clipW < 0.1f) return false;

    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;

    screen.x = (windowWidth / 2.0f) + (ndcX * windowWidth / 2.0f);
    screen.y = (windowHeight / 2.0f) - (ndcY * windowHeight / 2.0f);

    return true;
}

void DrawESP(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer) {
    float viewMatrix[16];
    ReadProcessMemory(hProcess, (LPCVOID)(moduleBase + Offsets::dwViewMatrix), viewMatrix, sizeof(viewMatrix), NULL);

    int localTeam = Memory::Read<int>(hProcess, localPlayer + Offsets::m_iTeamNum);

    uintptr_t entityList = Memory::Read<uintptr_t>(hProcess, moduleBase + Offsets::dwEntityList);

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    for (int i = 1; i < 64; ++i) {
        uintptr_t listEntry = Memory::Read<uintptr_t>(hProcess, entityList + i * 0x78);
        if (listEntry == 0) continue;
        uintptr_t player = Memory::Read<uintptr_t>(hProcess, listEntry + 0x10);
        if (player == 0) continue;

        int health = Memory::Read<int>(hProcess, player + Offsets::m_iHealth);
        if (health < 1 || health > 100) continue;

        int team = Memory::Read<int>(hProcess, player + Offsets::m_iTeamNum);
        if (team == localTeam) continue;

        Vector3 pos = Memory::Read<Vector3>(hProcess, player + Offsets::m_vecOrigin);
        Vector3 headPos = pos;
        headPos.z += 75.0f; // Approximate head height

        Vector2 screenPos, screenHead;
        if (WorldToScreen(pos, screenPos, viewMatrix, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y) &&
            WorldToScreen(headPos, screenHead, viewMatrix, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y)) {

            float height = screenPos.y - screenHead.y;
            float width = height / 2.0f;

            drawList->AddRect(ImVec2(screenHead.x - width / 2, screenHead.y), ImVec2(screenHead.x + width / 2, screenPos.y), IM_COL32(255, 0, 0, 255));
        }
    }
}
