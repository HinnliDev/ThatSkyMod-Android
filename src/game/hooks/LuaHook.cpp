#include <game/hooks/LuaHook.h>
#include <utils/hooking/dobby_wrapper.h>
#include <game/memory/offsets.h>
#include <game/memory/Address.h>
#include <game/memory/api.h>
#include <game/interop/LuaScriptQueue.h>
#include <progression/WaxRunner.h>
#include <utils/logging/log.h>
#include <cstdint>

namespace tsm { namespace game { namespace hooks { namespace lua {

namespace {
    // Sky 0.34.5 FUN_012c3720 (RVA 0x11C3720): LocalAvatarLoco update.
    // REMCP/Ghidra confirms x1 is Game* (uses Game+0x310 AvatarBarn and Game+0x948 LevelName).
    // It is reached from the locomotion update path and therefore provides a recurring game-thread pump.
    using LocalAvatarLocoUpdateFn = void(*)(void*, std::uintptr_t, std::uint32_t,
                                            std::uintptr_t, std::uintptr_t, std::uintptr_t,
                                            std::uintptr_t, std::uintptr_t, std::uint8_t);

    LocalAvatarLocoUpdateFn s_origLocalAvatarLocoUpdate = nullptr;

    extern "C" void LocalAvatarLocoUpdate_Hook(void* loco,
                                                std::uintptr_t game,
                                                std::uint32_t flags,
                                                std::uintptr_t a4,
                                                std::uintptr_t a5,
                                                std::uintptr_t a6,
                                                std::uintptr_t a7,
                                                std::uintptr_t a8,
                                                std::uint8_t a9) {
        if (s_origLocalAvatarLocoUpdate) {
            s_origLocalAvatarLocoUpdate(loco, game, flags, a4, a5, a6, a7, a8, a9);
        }

        void* L = tsm::game::api::LuaState();
        if (L) {
            tsm::lua::queue::ProcessNext(reinterpret_cast<std::uintptr_t>(L));
        }
        tsm::progression::WaxRunner::Get().Tick();
    }
}

bool Install() {
    if (tsm::game::memory::GetBase() == 0) {
        tsm::log::e("LuaHook: module base not initialized");
        return false;
    }

    if (tsm::game::Offsets::kLocalAvatarLocoUpdate == 0) {
        tsm::log::e("LuaHook: no verified game-thread pump for this offsets set");
        return false;
    }

    if (!tsm::utils::hooking::install_rva("LocalAvatarLocoUpdate",
                                           tsm::game::Offsets::kLocalAvatarLocoUpdate,
                                           (void*)LocalAvatarLocoUpdate_Hook,
                                           (void**)&s_origLocalAvatarLocoUpdate)) {
        tsm::log::e("LuaHook: LocalAvatarLocoUpdate hook failed");
        return false;
    }

    tsm::log::i("LuaHook: installed verified 0.34.5 game-thread pump at RVA 0x%lX",
                static_cast<unsigned long>(tsm::game::Offsets::kLocalAvatarLocoUpdate));
    return true;
}

}}}}
