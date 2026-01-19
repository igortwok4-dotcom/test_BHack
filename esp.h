#ifndef ESP_H
#define ESP_H

#include <windows.h>
#include <cstdint>  // для uintptr_t

struct Vector3 {
    float x, y, z;
};

struct Vector2 {
    float x, y;
};

namespace Config {
    extern bool espEnabled;
    // Можно добавить дополнительные настройки ESP, если нужно:
    // extern bool espBox;
    // extern bool espHealthBar;
    // extern bool espSnaplines;
    // extern ImColor espColorEnemy;
    // extern ImColor espColorTeam;
}

// Основная функция отрисовки ESP
void DrawESP(HANDLE hProcess, uintptr_t moduleBase, uintptr_t localPlayer);

// Вспомогательная функция преобразования мировых координат в экранные
bool WorldToScreen(const Vector3& worldPos, Vector2& screenPos, float matrix[16], int windowWidth, int windowHeight);

#endif // ESP_H
