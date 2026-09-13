#include <progression/PatchManager.h>
#include <game/memory/Address.h>
#include <game/memory/offsets.h>

namespace tsm { namespace progression {

PatchManager::~PatchManager() {
    RestoreAllPatches();
}

void PatchManager::ApplyPatches(bool include_plants, bool include_dyes) {
    // Wax/candle AOB signatures are intentionally not scanned on 0.34.5.
    ApplyWaxPatches();
    ApplyCandlePatches();
    if (include_plants) ApplyPlantPatches();
    if (include_dyes) ApplyDyePatches();
}

void PatchManager::RestoreAllPatches() {
    RestoreWaxPatches();
    RestoreCandlePatches();
    RestorePlantPatches();
    RestoreDyePatches();
}

bool PatchManager::ArePatchesApplied() const {
    return wax_applied_ || candles_applied_ || plants_applied_ || dyes_applied_;
}

void PatchManager::ApplyWaxPatches() {
    // Legacy kAutoCollectWaxPattern was not independently verified against 0.34.5.
    wax_patches_.clear();
    wax_applied_ = false;
}

void PatchManager::ApplyCandlePatches() {
    // Legacy kAutoBurnCandlesPattern was not independently verified against 0.34.5.
    candle_patches_.clear();
    candles_applied_ = false;
}

void PatchManager::ApplyPlantPatches() {
    if (plants_applied_) return;
    if (tsm::game::memory::GetBase() == 0) tsm::game::memory::InitializeBase();
    plant_patches_.clear();

    const std::uint8_t kToInstr[4] = { 0xE0, 0x03, 0x27, 0x1E };
    const std::uintptr_t kTargets[] = {
        tsm::game::Offsets::kAutoBurnPlants1,
        tsm::game::Offsets::kAutoBurnPlants2,
        tsm::game::Offsets::kAutoBurnPlants3
    };

    for (std::uintptr_t rva : kTargets) {
        void* target = tsm::game::memory::RvaToPtr(rva);
        if (!target) continue;
        std::uint32_t cur = *reinterpret_cast<std::uint32_t*>(target);
        if (cur == 0xBD424940u || cur == 0xBD424920u) {
            plant_patches_.emplace_back(tsm::game::memory::CreatePatch(target, kToInstr, 4));
        }
    }

    for (auto& patch : plant_patches_) if (!patch.applied) patch.Apply();
    plants_applied_ = !plant_patches_.empty();
}

void PatchManager::ApplyDyePatches() {
    if (dyes_applied_) return;
    if (tsm::game::memory::GetBase() == 0) tsm::game::memory::InitializeBase();
    dye_patches_.clear();

    const std::uint8_t kToInstr[4] = { 0xE0, 0x03, 0x27, 0x1E };
    const std::uintptr_t kTargets[] = {
        tsm::game::Offsets::kAutoBurnPlants1,
        tsm::game::Offsets::kAutoBurnPlants2,
        tsm::game::Offsets::kAutoBurnPlants3
    };

    for (std::uintptr_t rva : kTargets) {
        void* target = tsm::game::memory::RvaToPtr(rva);
        if (!target) continue;
        std::uint32_t cur = *reinterpret_cast<std::uint32_t*>(target);
        if (cur == 0xBD424940u || cur == 0xBD424920u) {
            dye_patches_.emplace_back(tsm::game::memory::CreatePatch(target, kToInstr, 4));
        }
    }

    for (auto& patch : dye_patches_) if (!patch.applied) patch.Apply();
    dyes_applied_ = !dye_patches_.empty();
}

void PatchManager::RestoreWaxPatches() {
    for (auto& patch : wax_patches_) if (patch.address && patch.applied) patch.Restore();
    wax_patches_.clear();
    wax_applied_ = false;
}

void PatchManager::RestoreCandlePatches() {
    for (auto& patch : candle_patches_) if (patch.address && patch.applied) patch.Restore();
    candle_patches_.clear();
    candles_applied_ = false;
}

void PatchManager::RestorePlantPatches() {
    for (auto& patch : plant_patches_) if (patch.address && patch.applied) patch.Restore();
    plant_patches_.clear();
    plants_applied_ = false;
}

void PatchManager::RestoreDyePatches() {
    for (auto& patch : dye_patches_) if (patch.address && patch.applied) patch.Restore();
    dye_patches_.clear();
    dyes_applied_ = false;
}

}}
