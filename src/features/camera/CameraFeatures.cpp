#include <features/manager/FeatureManager.h>
#include <state/ModState.h>
#include <game/memory/Address.h>
#include <game/memory/Patch.h>
#include <game/memory/Memory.h>
#include <game/memory/offsets.h>
#include <game/memory/api.h>
#include <utils/logging/log.h>

namespace tsm { namespace features {

namespace camera {

bool ReadCameraAngleX(float& outValue) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        outValue = *reinterpret_cast<float*>(whisker + tsm::game::Offsets::kCameraAngleX);
        return true;
    } catch (...) { return false; }
}

bool WriteCameraAngleX(float value) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        return tsm::game::memory::WriteBytes(reinterpret_cast<void*>(whisker + tsm::game::Offsets::kCameraAngleX), &value, sizeof(value));
    } catch (...) { return false; }
}

bool ReadCameraAngleY(float& outValue) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        outValue = *reinterpret_cast<float*>(whisker + tsm::game::Offsets::kCameraAngleY);
        return true;
    } catch (...) { return false; }
}

bool WriteCameraAngleY(float value) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        return tsm::game::memory::WriteBytes(reinterpret_cast<void*>(whisker + tsm::game::Offsets::kCameraAngleY), &value, sizeof(value));
    } catch (...) { return false; }
}

bool ReadCameraRotation(float& outValue) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        outValue = *reinterpret_cast<float*>(whisker + tsm::game::Offsets::kCameraRotation);
        return true;
    } catch (...) { return false; }
}

bool WriteCameraRotation(float value) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        return tsm::game::memory::WriteBytes(reinterpret_cast<void*>(whisker + tsm::game::Offsets::kCameraRotation), &value, sizeof(value));
    } catch (...) { return false; }
}

bool ReadCameraFOV(float& outValue) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        outValue = *reinterpret_cast<float*>(whisker + tsm::game::Offsets::kCameraFOV);
        return true;
    } catch (...) { return false; }
}

bool WriteCameraFOV(float value) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        return tsm::game::memory::WriteBytes(reinterpret_cast<void*>(whisker + tsm::game::Offsets::kCameraFOV), &value, sizeof(value));
    } catch (...) { return false; }
}

bool ReadCameraZoom(float& outValue) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        outValue = *reinterpret_cast<float*>(whisker + tsm::game::Offsets::kCameraZoom);
        return true;
    } catch (...) { return false; }
}

bool WriteCameraZoom(float value) {
    try {
        std::uintptr_t whisker = tsm::game::api::WhiskerCamera();
        if (whisker == 0) return false;
        return tsm::game::memory::WriteBytes(reinterpret_cast<void*>(whisker + tsm::game::Offsets::kCameraZoom), &value, sizeof(value));
    } catch (...) { return false; }
}

} // namespace camera

void FeatureManager::ApplyCameraFeatures() {
    auto& cam = tsm::state::ModState::Get().camera;
    tsm::game::memory::EnsureInitialized();

    if (tsm::game::Offsets::kEnableGameCamSnap == 0) {
        if (cam.disableSnap) tsm::log::w("Camera DisableSnap disabled: no verified Sky 0.34.5 RVA");
        cam.disableSnap = false;
    } else {
        tsm::game::memory::WriteByte(tsm::game::Offsets::kEnableGameCamSnap, cam.disableSnap ? 0 : 1);
    }

    // These two legacy AOB signatures were not independently verified against the
    // 0.34.5 REMCP image. Keep their controls in the off state rather than scanning
    // and patching a potentially unrelated instruction sequence.
    if (cam.lockCamPos) {
        tsm::log::w("Camera Lock Position disabled: legacy AOB is unverified for Sky 0.34.5");
        cam.lockCamPos = false;
    }
    if (cam.freeZoom) {
        tsm::log::w("Camera Free Zoom disabled: legacy AOB is unverified for Sky 0.34.5");
        cam.freeZoom = false;
    }
}

}}
