#pragma once

#include <cstdint>
#include <string>

namespace tsm { namespace game { namespace memory {

bool InitializeBase(const std::string& module_name = "libil2cpp.so");

std::uintptr_t GetBase();

inline void* RvaToPtr(std::uintptr_t rva) {
    const std::uintptr_t base = GetBase();
    if (base == 0 || rva == 0) return nullptr;
    return reinterpret_cast<void*>(base + rva);
}

template <typename T>
inline T* RvaToPtr(std::uintptr_t rva) {
    const std::uintptr_t base = GetBase();
    if (base == 0 || rva == 0) return nullptr;
    return reinterpret_cast<T*>(base + rva);
}

inline void EnsureInitialized() {
    if (GetBase() == 0) {
        InitializeBase();
    }
}

}}}
