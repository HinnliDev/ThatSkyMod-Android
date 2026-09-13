#include <features/manager/FeatureManager.h>
#include <state/ModState.h>
#include <game/memory/Patch.h>
#include <game/memory/Address.h>
#include <game/memory/Memory.h>
#include <game/memory/offsets.h>
#include <game/interop/LuaHelpers.h>
#include <progression/AutoWaxTools.h>
#include <utils/logging/log.h>
#include <vector>

namespace tsm { namespace features {

namespace {
    using namespace tsm::game::memory;
    using namespace tsm::game;

    constexpr float kSuperRunSpeedEnabled = 20.0f;
    constexpr float kSuperRunSpeedDisabled = 3.25f;

    bool RequireVerifiedRva(const char* feature, std::uintptr_t rva, bool& stateFlag) {
        if (rva != 0) return true;
        stateFlag = false;
        tsm::log::w("Feature '%s' is disabled: no verified Sky 0.34.5 RVA", feature);
        return false;
    }
}

FeatureManager& FeatureManager::Get() {
    static FeatureManager instance;
    return instance;
}

void FeatureManager::ApplyFeature(FeatureId id, bool enabled) {
    auto& state = tsm::state::ModState::Get();
    EnsureInitialized();

    switch (id) {
        case FeatureId::Invincibility:
            state.playerGeneral.invincibility = enabled;
            if (!RequireVerifiedRva("Invincibility", Offsets::kInvincibility, state.playerGeneral.invincibility)) break;
            WriteByte(Offsets::kInvincibility, enabled ? 1 : 0);
            break;
        case FeatureId::AntiRainDrain:
            state.playerGeneral.antiRainDrain = enabled;
            if (!RequireVerifiedRva("AntiRainDrain", Offsets::kDisableRainDrain, state.playerGeneral.antiRainDrain)) break;
            WriteByte(Offsets::kDisableRainDrain, enabled ? 1 : 0);
            break;
        case FeatureId::AntiKrill:
            state.playerGeneral.antiKrill = enabled;
            if (!RequireVerifiedRva("AntiKrill", Offsets::kDarkCreatureTame, state.playerGeneral.antiKrill)) break;
            WriteByte(Offsets::kDarkCreatureTame, enabled ? 1 : 0);
            break;
        case FeatureId::AntiAFK:
            state.playerGeneral.antiAFK = enabled;
            if (!RequireVerifiedRva("AntiAFK", Offsets::kAllowAfk, state.playerGeneral.antiAFK)) break;
            WriteByte(Offsets::kAllowAfk, enabled ? 0 : 1);
            break;
        case FeatureId::AutoCharge:
            state.playerGeneral.autoCharge = enabled;
            if (!RequireVerifiedRva("AutoCharge", Offsets::kAutoCharge, state.playerGeneral.autoCharge)) break;
            WriteByte(Offsets::kAutoCharge, enabled ? 1 : 0);
            break;
        case FeatureId::SuperRun:
            state.playerGeneral.superRun = enabled;
            if (!RequireVerifiedRva("SuperRun", Offsets::kRunSpeed, state.playerGeneral.superRun)) break;
            WriteFloat(Offsets::kRunSpeed, enabled ? kSuperRunSpeedEnabled : kSuperRunSpeedDisabled);
            break;
        case FeatureId::SuperLaunch:
            state.playerGeneral.superLaunch = enabled;
            if (!RequireVerifiedRva("SuperLaunch", Offsets::kSuperLaunch, state.playerGeneral.superLaunch)) break;
            WriteFloat(Offsets::kSuperLaunch, enabled ? 2.0f : 1.0f);
            break;
        case FeatureId::SuperSlidey:
            state.playerGeneral.superSlidey = enabled;
            if (!RequireVerifiedRva("SuperSlidey", Offsets::kSuperSlidey, state.playerGeneral.superSlidey)) break;
            WriteByte(Offsets::kSuperSlidey, enabled ? 1 : 0);
            break;
        case FeatureId::SuperFlight:
            // The legacy signature was not independently confirmed in REMCP for 0.34.5.
            state.playerGeneral.superFlight = false;
            tsm::log::w("Feature 'SuperFlight' is disabled: legacy signature is unverified for Sky 0.34.5");
            break;
        case FeatureId::FastFlap: {
            state.playerGeneral.fastFlap = enabled;
            if (!RequireVerifiedRva("FastFlap", Offsets::kFastFlap, state.playerGeneral.fastFlap)) break;
            constexpr std::uint32_t kFastFlapPatch = 0x1E349000u;
            static std::uint32_t s_fastFlapOriginal = 0;
            if (enabled) {
                if (s_fastFlapOriginal == 0) s_fastFlapOriginal = ReadU32(Offsets::kFastFlap);
                WriteU32(Offsets::kFastFlap, kFastFlapPatch);
            } else if (s_fastFlapOriginal != 0) {
                WriteU32(Offsets::kFastFlap, s_fastFlapOriginal);
            }
            break;
        }
        case FeatureId::UnlimitedFireworks: {
            state.playerGeneral.unlimitedFireworks = enabled;
            if (!RequireVerifiedRva("UnlimitedFireworks", Offsets::kFireworksCooldown, state.playerGeneral.unlimitedFireworks)) break;
            constexpr std::uint32_t kFireworksNOP = 0xD503201Fu;
            static std::uint32_t s_fireworksOriginal = 0;
            static bool s_fireworksOrigSet = false;
            if (enabled) {
                if (!s_fireworksOrigSet) {
                    s_fireworksOriginal = ReadU32(Offsets::kFireworksCooldown);
                    s_fireworksOrigSet = true;
                }
                WriteU32(Offsets::kFireworksCooldown, kFireworksNOP);
            } else if (s_fireworksOrigSet) {
                WriteU32(Offsets::kFireworksCooldown, s_fireworksOriginal);
                s_fireworksOrigSet = false;
            }
            break;
        }
        case FeatureId::UnlockAll:
            state.unlocks.unlockAll = false;
            tsm::log::w("Feature 'UnlockAll' is disabled: legacy signatures are unverified for Sky 0.34.5");
            ApplyUnlockFeatures();
            break;
        case FeatureId::UnlockEmoteLevels:
            state.unlocks.unlockEmoteLevels = false;
            tsm::log::w("Feature 'UnlockEmoteLevels' is disabled: legacy signature is unverified for Sky 0.34.5");
            ApplyUnlockFeatures();
            break;
        case FeatureId::UnlockRelationshipAbilities:
            state.unlocks.unlockRelationshipAbilities = enabled;
            if (!RequireVerifiedRva("UnlockRelationshipAbilities", Offsets::kEnableAllRelationshipAbilities,
                                    state.unlocks.unlockRelationshipAbilities)) break;
            ApplyUnlockFeatures();
            break;
        case FeatureId::AutoCollectWax:
            // Do not execute the legacy AOB until independently verified for this build.
            tsm::progression::SetAutoCollectWaxEnabled(false);
            tsm::log::w("Feature 'AutoCollectWax' is disabled: legacy signature is unverified for Sky 0.34.5");
            break;
        case FeatureId::AutoBurnCandles:
            tsm::progression::SetAutoBurnCandlesEnabled(false);
            tsm::log::w("Feature 'AutoBurnCandles' is disabled: legacy signature is unverified for Sky 0.34.5");
            break;
        case FeatureId::AutoBurnPlants:
            // This path additionally validates the expected instruction at every supplied RVA.
            tsm::progression::SetAutoBurnPlantsEnabled(enabled);
            break;
        case FeatureId::FastHome:
            state.world.fastHome = enabled;
            ApplyWorldFeatures();
            break;
        case FeatureId::ShowModdedOutfits:
            state.world.showModdedOutfits = enabled;
            ApplyWorldFeatures();
            break;
        case FeatureId::LightAllPlayers: {
            if (Offsets::kAvatarCharcoaling == 0) {
                tsm::log::w("Feature 'LightAllPlayers' is disabled: no verified Sky 0.34.5 RVA");
                break;
            }
            std::uint8_t b = enabled ? 0 : 1;
            WriteByte(Offsets::kAvatarCharcoaling, b);
            break;
        }
        case FeatureId::RevealAllPlayers: {
            if (Offsets::kRevealPlayers == 0) {
                tsm::log::w("Feature 'RevealAllPlayers' is disabled: no verified Sky 0.34.5 RVA");
                break;
            }
            static std::uint32_t s_revealPlayersOriginal = 0;
            static bool s_revealPlayersOrigSet = false;
            if (enabled) {
                if (!s_revealPlayersOrigSet) {
                    s_revealPlayersOriginal = ReadU32(Offsets::kRevealPlayers);
                    s_revealPlayersOrigSet = true;
                }
                WriteU32(Offsets::kRevealPlayers, Signatures::kNopInstruction);
            } else if (s_revealPlayersOrigSet) {
                WriteU32(Offsets::kRevealPlayers, s_revealPlayersOriginal);
                s_revealPlayersOrigSet = false;
            }
            break;
        }
        case FeatureId::EspPlayers:
            state.debug.espShowPlayers = enabled;
            break;
        case FeatureId::EspNPCs:
            state.debug.espShowNPCs = enabled;
            break;
        case FeatureId::EspWingLights:
            state.debug.espShowWingLights = enabled;
            break;
        case FeatureId::EspDyes:
            state.debug.espShowDyes = enabled;
            break;
        case FeatureId::EspCandles:
            state.debug.espShowCandles = enabled;
            break;
    }
}

void FeatureManager::ApplyPlayerFeatures() {
    auto& pg = tsm::state::ModState::Get().playerGeneral;
    ApplyFeature(FeatureId::Invincibility, pg.invincibility);
    ApplyFeature(FeatureId::AntiRainDrain, pg.antiRainDrain);
    ApplyFeature(FeatureId::AntiKrill, pg.antiKrill);
    ApplyFeature(FeatureId::AntiAFK, pg.antiAFK);
    ApplyFeature(FeatureId::AutoCharge, pg.autoCharge);
    ApplyFeature(FeatureId::SuperRun, pg.superRun);
    ApplyFeature(FeatureId::SuperSlidey, pg.superSlidey);
    ApplyFeature(FeatureId::SuperFlight, pg.superFlight);
    ApplyFeature(FeatureId::SuperLaunch, pg.superLaunch);
    ApplyFeature(FeatureId::FastFlap, pg.fastFlap);
    ApplyFeature(FeatureId::UnlimitedFireworks, pg.unlimitedFireworks);
}

void FeatureManager::ApplyAllFeatures() {
    ApplyPlayerFeatures();
    ApplyCameraFeatures();
    ApplyUnlockFeatures();
    ApplyWorldFeatures();
}

}}
