// esp.cpp
#include "esp.h"
#include "memory.h"
#include "offsets.h"
#include <iostream> // For console output, replace with overlay for real ESP

void DrawESP(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer) {
    int localTeam = Memory::Read<int>(hProcess, localPlayer + Offsets::m_iTeamNum);

    uintptr_t entityList = Memory::Read<uintptr_t>(hProcess, moduleBase + Offsets::dwEntityList);

    for (int i = 1; i < 64; ++i) { // Assume 64 players
        uintptr_t player = Memory::Read<uintptr_t>(hProcess, entityList + i * 0x10);
        if (player == 0) continue;

        int health = Memory::Read<int>(hProcess, player + Offsets::m_iHealth);
        if (health < 1 || health > 100) continue;

        int team = Memory::Read<int>(hProcess, player + Offsets::m_iTeamNum);
        if (team == localTeam) continue; // Same team

        Vector3 pos = Memory::Read<Vector3>(hProcess, player + Offsets::m_vecOrigin);

        // For console test: Print positions
        std::cout << "Enemy at (" << pos.x << ", " << pos.y << ", " << pos.z << ") Health: " << health << std::endl;

        // Real ESP: Would draw overlay here, but need ImGui or DirectX overlay (complex, not included)
    }
}
