#pragma once

#include <cstdint>
#include <type_traits>
#include <game/memory/Address.h>

namespace tsm::game::mem {

inline void* rva(std::uintptr_t rva) {
    const auto base = tsm::game::memory::GetBase();
    if (base == 0 || rva == 0) return nullptr;
    return reinterpret_cast<void*>(base + rva);
}

inline std::uintptr_t addr(std::uintptr_t rva) {
    const auto base = tsm::game::memory::GetBase();
    if (base == 0 || rva == 0) return 0;
    return base + rva;
}

inline std::uintptr_t add(void* p, std::uintptr_t off) {
    return reinterpret_cast<std::uintptr_t>(p) + off;
}

inline void* as_ptr(std::uintptr_t a) { return reinterpret_cast<void*>(a); }

inline void* read_ptr_abs(std::uintptr_t absolute) {
    return absolute ? *reinterpret_cast<void**>(absolute) : nullptr;
}

inline void* read_ptr_rva(std::uintptr_t rva) {
    return read_ptr_abs(addr(rva));
}

template <class T>
inline T read_abs(std::uintptr_t absolute) {
    static_assert(std::is_trivially_copyable_v<T>, "read_abs requires trivially copyable type");
    return absolute ? *reinterpret_cast<T*>(absolute) : T{};
}

template <class T>
inline T read_rva(std::uintptr_t rva) {
    return read_abs<T>(addr(rva));
}

template <class T>
inline void write_abs(std::uintptr_t absolute, const T& v) {
    static_assert(std::is_trivially_copyable_v<T>, "write_abs requires trivially copyable type");
    if (absolute) *reinterpret_cast<T*>(absolute) = v;
}

}
