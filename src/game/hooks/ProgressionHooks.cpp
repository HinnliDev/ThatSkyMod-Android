#include <game/hooks/ProgressionHooks.h>
#include <game/memory/offsets.h>
#include <game/memory/api.h>
#include <game/memory/mem.h>
#include <utils/logging/log.h>

namespace tsm { namespace game { namespace hooks { namespace progression {
namespace {
std::uintptr_t ReadGameField(std::uintptr_t field) {
    void* game = tsm::game::api::Game();
    if (!game || field == 0) return 0;
    return tsm::game::mem::read_abs<std::uintptr_t>(tsm::game::mem::add(game, field));
}
}

bool Install() {
    tsm::log::i("ProgressionHooks: verified Game fields enabled; unverified code hooks disabled");
    return true;
}

std::uintptr_t GetWingBuffBarn() { return ReadGameField(tsm::game::Offsets::kWingBuffBarn); }
std::uintptr_t GetFirstWing() { return 0; }
int* GetWingCountPtr() { return nullptr; }
std::uintptr_t GetRadianceBarn() { return ReadGameField(tsm::game::Offsets::kRadianceBarnField); }
std::uintptr_t GetFirstDye() { return 0; }
int* GetDyeCountPtr() { return nullptr; }

}}}}
