#ifndef MEMORY_H
#define MEMORY_H

#include <windows.h>
#include <string>
#include <cstdint>  // для uintptr_t

// Основные структуры, которые используются в проекте
struct Vector3 {
    float x, y, z;

    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }
};

// Пространство имён для всех функций чтения памяти
namespace Memory {
    // Шаблонная функция для безопасного чтения любого типа из памяти процесса
    template <typename T>
    T Read(HANDLE hProcess, uintptr_t address) {
        T value = {};
        if (hProcess && address) {
            ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), nullptr);
        }
        return value;
    }

    // Специализированные версии (для ускорения компиляции и избежания предупреждений)
    template <> float Read<float>(HANDLE hProcess, uintptr_t address);
    template <> int Read<int>(HANDLE hProcess, uintptr_t address);
    template <> uint32_t Read<uint32_t>(HANDLE hProcess, uintptr_t address);
    template <> uint64_t Read<uint64_t>(HANDLE hProcess, uintptr_t address);
    template <> uintptr_t Read<uintptr_t>(HANDLE hProcess, uintptr_t address);
    template <> Vector3 Read<Vector3>(HANDLE hProcess, uintptr_t address);

    // Поиск сигнатуры (pattern scan) в модуле игры
    // sig - байты сигнатуры (с \x00 для неизвестных)
    // mask - маска ('x' = известный байт, '?' = пропустить)
    uintptr_t FindSignature(
        HANDLE hProcess,
        uintptr_t startAddress,
        size_t regionSize,
        const std::string& signature,
        const std::string& mask
    );

    // Получение базового адреса модуля по имени (cs2.exe)
    uintptr_t GetModuleBaseAddress(DWORD processId, const std::string& moduleName);
}

#endif // MEMORY_H
