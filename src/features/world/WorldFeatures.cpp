#include <features/manager/FeatureManager.h>
#include <state/ModState.h>
#include <game/memory/Address.h>
#include <game/memory/Memory.h>
#include <game/memory/offsets.h>
#include <game/interop/LuaScriptQueue.h>
#include <utils/logging/log.h>

namespace tsm { namespace features {

namespace {
    using namespace tsm::game::memory;
    using namespace tsm::game;

    std::uint32_t g_fastHomeOrig = 0;
    bool g_fastHomeOrigSet = false;
    std::uint32_t g_windWallsOrig = 0;
    bool g_windWallsOrigSet = false;
    std::uint32_t g_readTableMessagesOrig = 0;
    bool g_readTableMessagesOrigSet = false;

    void DisableUnavailable(const char* name, std::uintptr_t rva, bool& flag) {
        if (rva != 0) return;
        if (flag) tsm::log::w("World feature '%s' is disabled: no verified Sky 0.34.5 RVA", name);
        flag = false;
    }
}

void FeatureManager::ApplyWorldFeatures() {
    auto& world = tsm::state::ModState::Get().world;
    EnsureInitialized();

    DisableUnavailable("DisableSpiritGates", Offsets::kDisableGates, world.disableSpiritGates);
    DisableUnavailable("PauseUiAnimations", Offsets::kTguiPauseAnimation, world.pauseUiAnimations);
    DisableUnavailable("FastHome", Offsets::kFastHome, world.fastHome);
    DisableUnavailable("DisableWindWalls", Offsets::kDisableWindWall, world.disableWindWalls);
    DisableUnavailable("ReadTableMessages", Offsets::kReadTableMessages, world.readTableMessages);
    DisableUnavailable("ShowSpirits", Offsets::kDebugShowSpiritLocations, world.showSpirits);
    DisableUnavailable("ShowWingBuffs", Offsets::kShowRadarForPreviousWingBuffs, world.showWingBuffs);
    DisableUnavailable("ShowMapShrines", Offsets::kEnableShrineRadar, world.showMapShrines);
    DisableUnavailable("ShowMapItems", Offsets::kShowAllFeedback, world.showMapItems);
    DisableUnavailable("ShowModdedOutfits", Offsets::kDisableRemoteOutfitCache, world.showModdedOutfits);

    if (Offsets::kDisableGates) WriteByte(Offsets::kDisableGates, world.disableSpiritGates ? 1 : 0);
    if (Offsets::kTguiPauseAnimation) WriteByte(Offsets::kTguiPauseAnimation, world.pauseUiAnimations ? 1 : 0);

    if (world.fastHome) {
        if (!g_fastHomeOrigSet) {
            g_fastHomeOrig = ReadU32(Offsets::kFastHome);
            g_fastHomeOrigSet = true;
        }
        WriteU32(Offsets::kFastHome, Signatures::kNopInstruction);
    } else if (g_fastHomeOrigSet && Offsets::kFastHome) {
        WriteU32(Offsets::kFastHome, g_fastHomeOrig);
        g_fastHomeOrigSet = false;
    }

    // Lua-backed functionality remains enabled: the queue is pumped from the verified
    // LocalAvatarLoco update hook on the game thread.
    tsm::lua::queue::Enqueue(world.disableCutscenes
        ? "Vars.Timeline.kAutoSkipAllTimelines(true)"
        : "Vars.Timeline.kAutoSkipAllTimelines(false)");

    if (world.disableWindWalls) {
        if (!g_windWallsOrigSet) {
            g_windWallsOrig = ReadU32(Offsets::kDisableWindWall);
            g_windWallsOrigSet = true;
        }
        WriteU32(Offsets::kDisableWindWall, Signatures::kMovW13Zero);
    } else if (g_windWallsOrigSet && Offsets::kDisableWindWall) {
        WriteU32(Offsets::kDisableWindWall, g_windWallsOrig);
        g_windWallsOrigSet = false;
    }

    if (world.readTableMessages) {
        if (!g_readTableMessagesOrigSet) {
            g_readTableMessagesOrig = ReadU32(Offsets::kReadTableMessages);
            g_readTableMessagesOrigSet = true;
        }
        WriteU32(Offsets::kReadTableMessages, Signatures::kNopInstruction);
    } else if (g_readTableMessagesOrigSet && Offsets::kReadTableMessages) {
        WriteU32(Offsets::kReadTableMessages, g_readTableMessagesOrig);
        g_readTableMessagesOrigSet = false;
    }

    if (Offsets::kDebugShowSpiritLocations) WriteByte(Offsets::kDebugShowSpiritLocations, world.showSpirits ? 1 : 0);
    if (Offsets::kShowRadarForPreviousWingBuffs) WriteByte(Offsets::kShowRadarForPreviousWingBuffs, world.showWingBuffs ? 1 : 0);
    if (Offsets::kEnableShrineRadar) WriteByte(Offsets::kEnableShrineRadar, world.showMapShrines ? 1 : 0);
    if (Offsets::kShowAllFeedback) WriteByte(Offsets::kShowAllFeedback, world.showMapItems ? 1 : 0);
    if (Offsets::kDisableRemoteOutfitCache) WriteByte(Offsets::kDisableRemoteOutfitCache, world.showModdedOutfits ? 1 : 0);
}

}}
