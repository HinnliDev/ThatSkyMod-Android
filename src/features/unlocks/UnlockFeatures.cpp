#include <features/manager/FeatureManager.h>
#include <state/ModState.h>
#include <game/memory/Address.h>
#include <game/memory/Memory.h>
#include <game/memory/offsets.h>
#include <utils/logging/log.h>

namespace tsm { namespace features {

void FeatureManager::ApplyUnlockFeatures() {
    auto& unlocks = tsm::state::ModState::Get().unlocks;
    tsm::game::memory::EnsureInitialized();

    // The legacy AOBs for UnlockAll/EmoteLevels were not independently confirmed
    // against the 0.34.5 Bootloader in REMCP. Keep them visibly/off-state fail closed
    // instead of scanning and patching an unverified location.
    if (unlocks.unlockAll) {
        tsm::log::w("UnlockAll disabled: legacy signature is unverified for Sky 0.34.5");
        unlocks.unlockAll = false;
    }
    if (unlocks.unlockEmoteLevels) {
        tsm::log::w("UnlockEmoteLevels disabled: legacy signature is unverified for Sky 0.34.5");
        unlocks.unlockEmoteLevels = false;
    }

    if (tsm::game::Offsets::kEnableAllRelationshipAbilities == 0) {
        if (unlocks.unlockRelationshipAbilities) {
            tsm::log::w("UnlockRelationshipAbilities disabled: no verified Sky 0.34.5 RVA");
        }
        unlocks.unlockRelationshipAbilities = false;
        return;
    }

    tsm::game::memory::WriteByte(tsm::game::Offsets::kEnableAllRelationshipAbilities,
                                 unlocks.unlockRelationshipAbilities ? 1 : 0);
}

}}
