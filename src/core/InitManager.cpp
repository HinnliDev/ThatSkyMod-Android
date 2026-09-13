#include <core/InitManager.h>
#include <core/UpdateChecker.h>
#include <game/memory/Address.h>
#include <game/memory/Memory.h>
#include <game/memory/offsets.h>
#include <game/memory/RuntimeOffsets.h>
#include <game/hooks/HookManager.h>
#include <game/interop/LuaFunctions.h>
#include <data/DataManager.h>
#include <ui/core/App.h>
#include <utils/logging/log.h>

namespace tsm { namespace core {

InitManager& InitManager::Get() {
    static InitManager instance;
    return instance;
}

bool InitManager::IsEarlyInitialized() const { return m_earlyInit; }
bool InitManager::IsLateInitialized() const { return m_lateInit; }
bool InitManager::AreHooksInstalled() const { return m_hooksInstalled; }
bool InitManager::IsUIInitialized() const { return m_uiInitialized; }

void InitManager::PerformEarlyInit() {
    if (m_earlyInit) return;
    tsm::log::init("TSM");
    tsm::log::set_enabled(false);
    // No native hook is installed here. Module base and Canvas runtime offsets are
    // established first in PerformLateInit(), before any hook installation.
    m_earlyInit = true;
}

void InitManager::PerformLateInit() {
    if (m_lateInit) return;

    tsm::game::memory::InitializeBase();
    tsm::game::runtime_offsets::LoadCanvasOffsets();

    if (tsm::game::Offsets::kStarwatchAuth != 0) {
        tsm::game::memory::WriteU32(tsm::game::Offsets::kStarwatchAuth,
                                    tsm::game::Signatures::kRetInstruction);
    } else {
        tsm::log::w("InitManager: StarwatchAuth patch disabled: no verified 0.34.5 RVA");
    }

    tsm::lua::functions::InitializeCore();
    tsm::lua::functions::InitializeExtended();
    tsm::data::DataManager::Get().LoadAll();

    if (!m_hooksInstalled) InstallHooks();
    m_lateInit = true;
}

bool InitManager::InstallHooks() {
    if (m_hooksInstalled) return true;
    if (tsm::game::memory::GetBase() == 0) tsm::game::memory::InitializeBase();

    if (!tsm::game::hooks::HookManager::Get().InstallAll()) {
        tsm::log::e("InitManager: Game hooks install failed");
        return false;
    }

    tsm::log::i("InitManager: Game hooks installed successfully");
    m_hooksInstalled = true;
    return true;
}

void InitManager::InitializeUI() {
    if (m_uiInitialized) return;
    tsm::ui::Initialize();
    m_uiInitialized = true;
    tsm::log::i("InitManager: UI system initialized");
    tsm::core::UpdateChecker::Get().CheckForUpdates();
}

}}
