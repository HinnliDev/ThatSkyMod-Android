#include <array>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

#include <Cipher/CipherUtils.h>
#include <game/memory/Address.h>
#include <game/memory/Memory.h>
#include <game/memory/Patch.h>
#include <game/memory/api.h>
#include <game/memory/mem.h>
#include <game/memory/offsets.h>
#include <utils/common/vec3.h>

namespace tsm::game::api {

[[nodiscard]] bool PlayerUuid::IsValid() const noexcept {
    for (int i = 0; i < 16; ++i) {
        if (bytes[i] != 0) return true;
    }
    return false;
}

[[nodiscard]] std::string PlayerUuid::ToString() const {
    char buf[64];
    std::snprintf(buf, sizeof(buf),
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        bytes[0], bytes[1], bytes[2], bytes[3],
        bytes[4], bytes[5], bytes[6], bytes[7],
        bytes[8], bytes[9],
        bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]
    );
    return std::string(buf);
}

namespace detail {
    std::array<std::uintptr_t, kMaxAvatars> s_avatarCache{};
    bool s_avatarCacheValid = false;
}

void InvalidateAvatarCache() noexcept {
    detail::s_avatarCache.fill(0);
    detail::s_avatarCacheValid = false;
}

void RefreshAvatarCache() {
    if (Offsets::kAvatarLocalSlot == 0 || Offsets::kAvatarSlotStride == 0) {
        InvalidateAvatarCache();
        return;
    }

    auto barn = AvatarBarn();
    if (!barn) {
        InvalidateAvatarCache();
        return;
    }

    const std::uintptr_t barnAddr = reinterpret_cast<std::uintptr_t>(barn);
    for (int i = 0; i < kMaxAvatars; ++i) {
        detail::s_avatarCache[i] = barnAddr +
            Offsets::kAvatarSlotStride * static_cast<std::uintptr_t>(i) +
            Offsets::kAvatarLocalSlot;
    }
    detail::s_avatarCacheValid = true;
}

[[nodiscard]] std::uintptr_t GetCachedAvatar(int index) {
    if (index < 0 || index >= kMaxAvatars) return 0;
    if (!detail::s_avatarCacheValid) RefreshAvatarCache();
    if (!detail::s_avatarCacheValid) return 0;
    return detail::s_avatarCache[index];
}

[[nodiscard]] void* Game() {
    return mem::read_ptr_rva(Offsets::Game);
}

[[nodiscard]] void* LuaState() {
    auto g = Game();
    if (!g || Offsets::kLuaState == 0) return nullptr;
    return mem::read_ptr_abs(mem::add(g, Offsets::kLuaState));
}

[[nodiscard]] void* AvatarBarn() {
    auto g = Game();
    if (!g || Offsets::kAvatarBarn == 0) return nullptr;
    return mem::read_ptr_abs(mem::add(g, Offsets::kAvatarBarn));
}

[[nodiscard]] void* NetPlayerBarn() {
    auto g = Game();
    if (!g || Offsets::kNetPlayerBarnPtr == 0 || Offsets::kNetPlayerBarnOffset == 0) return nullptr;

    void* ptr = mem::read_ptr_abs(mem::add(g, Offsets::kNetPlayerBarnPtr));
    if (!ptr) return nullptr;
    return mem::read_ptr_abs(mem::add(ptr, Offsets::kNetPlayerBarnOffset));
}

[[nodiscard]] const char* LevelName() {
    auto g = Game();
    if (!g || Offsets::kLevelName == 0) return nullptr;
    const std::uintptr_t address = mem::add(g, Offsets::kLevelName);
    return address ? *reinterpret_cast<const char**>(address) : nullptr;
}

[[nodiscard]] void* LocalAvatar(int index) {
    if (index < 0 || index >= kMaxAvatars || Offsets::kAvatarLocalSlot == 0 || Offsets::kAvatarSlotStride == 0) return nullptr;
    auto barn = AvatarBarn();
    if (!barn) return nullptr;

    const std::uintptr_t slot = Offsets::kAvatarLocalSlot +
        static_cast<std::uintptr_t>(index) * Offsets::kAvatarSlotStride;
    return mem::read_ptr_abs(mem::add(barn, slot));
}

[[nodiscard]] void* LocalAvatarOutfit(int index) {
    if (index < 0 || index >= kMaxAvatars || Offsets::kAvatarLocalSlot == 0 ||
        Offsets::kAvatarSlotStride == 0 || Offsets::kAvatarOutfit == 0) return nullptr;
    auto barn = AvatarBarn();
    if (!barn) return nullptr;

    const std::uintptr_t offset = Offsets::kAvatarLocalSlot +
        static_cast<std::uintptr_t>(index) * Offsets::kAvatarSlotStride +
        Offsets::kAvatarOutfit;
    return mem::read_ptr_abs(reinterpret_cast<std::uintptr_t>(barn) + offset);
}

[[nodiscard]] vec3 LocalAvatarPosition(int index) {
    if (index < 0 || index >= kMaxAvatars || Offsets::kAvatarLocalSlot == 0 ||
        Offsets::kAvatarSlotStride == 0 || Offsets::kAvatarPosition == 0) return vec3{0.0f, 0.0f, 0.0f};
    auto barn = AvatarBarn();
    if (!barn) return vec3{0.0f, 0.0f, 0.0f};

    const std::uintptr_t offset = Offsets::kAvatarLocalSlot +
        static_cast<std::uintptr_t>(index) * Offsets::kAvatarSlotStride +
        Offsets::kAvatarPosition;
    float* p = *reinterpret_cast<float**>(reinterpret_cast<std::uintptr_t>(barn) + offset);
    if (!p) return vec3{0.0f, 0.0f, 0.0f};
    return vec3{p[0], p[1], p[2]};
}

[[nodiscard]] float LocalAvatarRotation(int index) {
    if (index < 0 || index >= kMaxAvatars || Offsets::kAvatarLocalSlot == 0 ||
        Offsets::kAvatarSlotStride == 0 || Offsets::kAvatarPosition == 0) return 0.0f;
    auto barn = AvatarBarn();
    if (!barn) return 0.0f;

    const std::uintptr_t offset = Offsets::kAvatarLocalSlot +
        static_cast<std::uintptr_t>(index) * Offsets::kAvatarSlotStride +
        Offsets::kAvatarPosition;
    float* p = *reinterpret_cast<float**>(reinterpret_cast<std::uintptr_t>(barn) + offset);
    return p ? p[8] : 0.0f;
}

[[nodiscard]] float* LocalAvatarPositionRawPtr(int index) {
    if (index < 0 || index >= kMaxAvatars || Offsets::kAvatarLocalSlot == 0 ||
        Offsets::kAvatarSlotStride == 0 || Offsets::kAvatarPosition == 0) return nullptr;
    auto barn = AvatarBarn();
    if (!barn) return nullptr;

    const std::uintptr_t offset = Offsets::kAvatarLocalSlot +
        static_cast<std::uintptr_t>(index) * Offsets::kAvatarSlotStride +
        Offsets::kAvatarPosition;
    return *reinterpret_cast<float**>(reinterpret_cast<std::uintptr_t>(barn) + offset);
}

[[nodiscard]] bool ShouldDisplay(std::uintptr_t avatar) {
    if (avatar == 0 || Offsets::kShouldDisplay == 0 || tsm::game::memory::GetBase() == 0) return false;
    using ShouldDisplayFunc = bool(*)(std::uintptr_t);
    auto func = reinterpret_cast<ShouldDisplayFunc>(tsm::game::memory::GetBase() + Offsets::kShouldDisplay);
    return func(avatar);
}

[[nodiscard]] AvatarInfo GetAvatarInfo(std::uintptr_t avatar) {
    AvatarInfo info{};
    if (avatar == 0 || Offsets::kAvatarOutfit == 0 || Offsets::kAvatarPosition == 0 || Offsets::kAvatarShout == 0) return info;

    try {
        const std::uintptr_t outfitPtr = *reinterpret_cast<std::uintptr_t*>(avatar + Offsets::kAvatarOutfit);
        const std::uintptr_t posPtr = *reinterpret_cast<std::uintptr_t*>(avatar + Offsets::kAvatarPosition);
        if (posPtr != 0) info.avatarPosition = reinterpret_cast<vec3*>(posPtr);
        info.avatarShout = *reinterpret_cast<std::uintptr_t*>(avatar + Offsets::kAvatarShout);

        const bool outfitLayoutVerified = Offsets::kHeight != 0 && Offsets::kScale != 0 &&
            Offsets::kVoice != 0 && Offsets::kStance != 0 && Offsets::kHair != 0 &&
            Offsets::kHat != 0 && Offsets::kMask != 0 && Offsets::kFace != 0 &&
            Offsets::kNeck != 0 && Offsets::kBody != 0 && Offsets::kFeet != 0 &&
            Offsets::kWing != 0 && Offsets::kProp != 0;
        if (outfitPtr != 0 && outfitLayoutVerified) {
            info.avatarHeight = reinterpret_cast<float*>(outfitPtr + Offsets::kHeight);
            info.avatarScale = reinterpret_cast<float*>(outfitPtr + Offsets::kScale);
            info.avatarVoice = reinterpret_cast<std::uint8_t*>(outfitPtr + Offsets::kVoice);
            info.avatarStance = reinterpret_cast<std::uint8_t*>(outfitPtr + Offsets::kStance);
            info.avatarHair = reinterpret_cast<const char*>(outfitPtr + Offsets::kHair);
            info.avatarHat = reinterpret_cast<const char*>(outfitPtr + Offsets::kHat);
            info.avatarMask = reinterpret_cast<const char*>(outfitPtr + Offsets::kMask);
            info.avatarFace = reinterpret_cast<const char*>(outfitPtr + Offsets::kFace);
            info.avatarNeck = reinterpret_cast<const char*>(outfitPtr + Offsets::kNeck);
            info.avatarBody = reinterpret_cast<const char*>(outfitPtr + Offsets::kBody);
            info.avatarFeet = reinterpret_cast<const char*>(outfitPtr + Offsets::kFeet);
            info.avatarWing = reinterpret_cast<const char*>(outfitPtr + Offsets::kWing);
            info.avatarProp = reinterpret_cast<const char*>(outfitPtr + Offsets::kProp);
        }
    } catch (...) {
    }
    return info;
}

[[nodiscard]] AvatarInfo GetAvatarInfoByIndex(int index) {
    return GetAvatarInfo(GetCachedAvatar(index));
}

[[nodiscard]] int GetPlayerIdByIndex(int index) {
    if (index < 0 || index >= kMaxAvatars || Offsets::kFirstPlayerIdOffset == 0 || Offsets::kPlayerIdStride == 0) return 0;
    void* npb = NetPlayerBarn();
    if (!npb) return 0;

    const std::uintptr_t playerIdAddr = mem::add(npb,
        Offsets::kFirstPlayerIdOffset + (Offsets::kPlayerIdStride * static_cast<std::uintptr_t>(index)));
    return playerIdAddr ? *reinterpret_cast<int*>(playerIdAddr) : 0;
}

[[nodiscard]] float* GameSpeedDeltaPtr() {
    if (Offsets::AudienceBarn == 0 || Offsets::kGameSpeedBarn == 0 || Offsets::kGameSpeedDelta == 0) return nullptr;
    void* pBarnLoc = tsm::game::memory::RvaToPtr(Offsets::AudienceBarn);
    if (!pBarnLoc) return nullptr;
    void* barn = *reinterpret_cast<void**>(pBarnLoc);
    if (!barn) return nullptr;
    void* gs = mem::read_ptr_abs(mem::add(barn, Offsets::kGameSpeedBarn));
    if (!gs) return nullptr;
    const std::uintptr_t delta = mem::add(gs, Offsets::kGameSpeedDelta);
    return delta ? reinterpret_cast<float*>(delta) : nullptr;
}

[[nodiscard]] float GameSpeed() {
    if (float* p = GameSpeedDeltaPtr()) return *p;
    return 1.0f;
}

void SetGameSpeed(float speed) {
    if (float* p = GameSpeedDeltaPtr()) tsm::game::memory::WriteBytes(p, &speed, sizeof(float));
}

[[nodiscard]] std::uintptr_t CameraSystem() {
    if (Offsets::AudienceBarn == 0 || Offsets::kCameraSystem == 0) return 0;
    void* pBarnLoc = tsm::game::memory::RvaToPtr(Offsets::AudienceBarn);
    if (!pBarnLoc) return 0;
    void* audience = *reinterpret_cast<void**>(pBarnLoc);
    if (!audience) return 0;
    return reinterpret_cast<std::uintptr_t>(mem::read_ptr_abs(mem::add(audience, Offsets::kCameraSystem)));
}

[[nodiscard]] std::uintptr_t WhiskerCamera() {
    if (Offsets::kWhiskerCamera == 0) return 0;
    const std::uintptr_t camSys = CameraSystem();
    return camSys ? camSys + Offsets::kWhiskerCamera : 0;
}

namespace {
    void* GetCandleBarnInternal() {
        if (Offsets::CandleBarn == 0) return nullptr;
        void* pBarnLoc = tsm::game::memory::RvaToPtr(Offsets::CandleBarn);
        if (!pBarnLoc) return nullptr;
        return *reinterpret_cast<void**>(pBarnLoc);
    }
}

[[nodiscard]] int CandleCount() {
    void* barn = GetCandleBarnInternal();
    if (!barn) return 0;
    int* pCount = reinterpret_cast<int*>(reinterpret_cast<char*>(barn) + 0x5A010);
    int count = *pCount;
    return count < 0 ? 0 : count;
}

[[nodiscard]] std::uintptr_t FirstCandle() {
    void* barn = GetCandleBarnInternal();
    return barn ? reinterpret_cast<std::uintptr_t>(reinterpret_cast<char*>(barn) + 0x40) : 0;
}

[[nodiscard]] int CandleTypeIndex(int index) {
    void* barn = GetCandleBarnInternal();
    if (!barn || index < 0) return -1;
    int count = CandleCount();
    if (index >= count) return -1;
    char* base = reinterpret_cast<char*>(barn);
    std::uintptr_t entryOffset = 0x1F810u + static_cast<std::uintptr_t>(index) * 0xD0u;
    return static_cast<int>(*reinterpret_cast<std::uint8_t*>(base + entryOffset + 0x9Cu));
}

[[nodiscard]] bool CandleIsActive(int index) {
    void* barn = GetCandleBarnInternal();
    if (!barn || index < 0) return false;
    int count = CandleCount();
    if (index >= count) return false;
    char* base = reinterpret_cast<char*>(barn);
    std::uintptr_t entryOffset = 0x1F810u + static_cast<std::uintptr_t>(index) * 0xD0u;
    void* node = *reinterpret_cast<void**>(base + entryOffset + 0x90u);
    if (!node) return false;
    std::uint16_t flags = *reinterpret_cast<std::uint16_t*>(reinterpret_cast<char*>(node) + 97);
    return (flags & 0x30u) == 0x30u;
}

}
