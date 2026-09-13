#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Cipher/CipherUtils.h>
#include <nlohmann/json.hpp>
#include <game/memory/offsets.h>
#include <utils/logging/log.h>

namespace tsm::game::runtime_offsets {

inline constexpr const char* kSupportedVersion = "0.34.5";
inline constexpr const char* kCanvasOffsetsFile = "libTSM_offsets.json";
inline constexpr std::uintptr_t kMaxAcceptedOffset = 0x04000000;

inline std::string CanvasOffsetsPath() {
    const char* configPath = CipherUtils::get_ConfigsPath();
    if (!configPath || !*configPath) return {};
    std::string path(configPath);
    if (!path.empty() && path.back() != '/') path.push_back('/');
    path += kCanvasOffsetsFile;
    return path;
}

inline std::unordered_map<std::string, std::uintptr_t*> KnownOffsets() {
    using namespace tsm::game::Offsets;
    return {
        {"Game", &Game}, {"AudienceBarn", &AudienceBarn}, {"CandleBarn", &CandleBarn},
        {"kLuaState", &kLuaState}, {"kGameInit", &kGameInit},
        {"kSystemAccountsAndroid", &kSystemAccountsAndroid},
        {"kAccountServerClient", &kAccountServerClient}, {"kAccountBarnGameField", &kAccountBarnGameField},
        {"kControllerGameField", &kControllerGameField}, {"kWingBuffBarn", &kWingBuffBarn},
        {"kCandleBarnField", &kCandleBarnField}, {"kTimelineBarn", &kTimelineBarn},
        {"kEventBarn", &kEventBarn}, {"kDarkstoneBarn", &kDarkstoneBarn},
        {"kRadianceBarnField", &kRadianceBarnField}, {"kDarkCreatureBarn", &kDarkCreatureBarn},
        {"kFriendBarn", &kFriendBarn}, {"kFriendDataBarn", &kFriendDataBarn},
        {"kAffinityBarn", &kAffinityBarn}, {"kPickupEmitterBarn", &kPickupEmitterBarn},
        {"kPickupBarn", &kPickupBarn}, {"kWaxChunkBarn", &kWaxChunkBarn},
        {"kPickupNodeBarn", &kPickupNodeBarn}, {"kMusicBarn", &kMusicBarn},
        {"kSheetMusicBarn", &kSheetMusicBarn}, {"kLevelStateBarn", &kLevelStateBarn},
        {"kGameMode", &kGameMode}, {"kServerHostname", &kServerHostname},
        {"kAvatarBarn", &kAvatarBarn}, {"kNetPlayerBarnPtr", &kNetPlayerBarnPtr},
        {"kNetPlayerBarnOffset", &kNetPlayerBarnOffset}, {"kFirstPlayerIdOffset", &kFirstPlayerIdOffset},
        {"kFirstPlayerUuidOffset", &kFirstPlayerUuidOffset}, {"kPlayerIdStride", &kPlayerIdStride},
        {"kAccountBarn", &kAccountBarn}, {"kAvatarOutfit", &kAvatarOutfit},
        {"kAvatarLocalSlot", &kAvatarLocalSlot}, {"kAvatarSlotStride", &kAvatarSlotStride},
        {"kAvatarPosition", &kAvatarPosition}, {"kAvatarShout", &kAvatarShout},
        {"kLevelName", &kLevelName}, {"kLoginType", &kLoginType}, {"kLoadingType", &kLoadingType},
        {"kShouldRestart", &kShouldRestart}, {"kGameSpeedBarn", &kGameSpeedBarn},
        {"kGameSpeedDelta", &kGameSpeedDelta}, {"kCameraSystem", &kCameraSystem},
        {"kCameraIntermediate", &kCameraIntermediate}, {"kViewProjectionMatrix", &kViewProjectionMatrix},
        {"kJitterFullHalf", &kJitterFullHalf}, {"kWhiskerCamera", &kWhiskerCamera},
        {"kCameraAngleX", &kCameraAngleX}, {"kCameraAngleY", &kCameraAngleY},
        {"kCameraRotation", &kCameraRotation}, {"kCameraFOV", &kCameraFOV}, {"kCameraZoom", &kCameraZoom},
        {"kHeight", &kHeight}, {"kScale", &kScale}, {"kVoice", &kVoice}, {"kStance", &kStance},
        {"kBody", &kBody}, {"kWing", &kWing}, {"kHair", &kHair}, {"kMask", &kMask},
        {"kNeck", &kNeck}, {"kFeet", &kFeet}, {"kHorn", &kHorn}, {"kFace", &kFace},
        {"kProp", &kProp}, {"kHat", &kHat}, {"kLuaDebugDoString", &kLuaDebugDoString},
        {"kLocalAvatarLocoUpdate", &kLocalAvatarLocoUpdate}, {"kLuaPushLightUserData", &kLuaPushLightUserData},
        {"kWingBuffUpdate", &kWingBuffUpdate}, {"kRadianceBarn", &kRadianceBarn},
        {"kAccountServerSetSession", &kAccountServerSetSession}, {"kHttpClientSetUserAgent", &kHttpClientSetUserAgent},
        {"kShouldDisplay", &kShouldDisplay}, {"kDoShout", &kDoShout},
        {"kSetJoinableFunction", &kSetJoinableFunction}, {"kPlayMusicKey", &kPlayMusicKey},
        {"kStopNote", &kStopNote}, {"kPianoFrame", &kPianoFrame},
        {"kSharedMemoryPlayPianoSound", &kSharedMemoryPlayPianoSound},
        {"kInvincibility", &kInvincibility}, {"kAutoCharge", &kAutoCharge},
        {"kDisableRainDrain", &kDisableRainDrain}, {"kDarkCreatureTame", &kDarkCreatureTame},
        {"kAllowAfk", &kAllowAfk}, {"kRunSpeed", &kRunSpeed}, {"kSuperSlidey", &kSuperSlidey},
        {"kAutoCollectAllFragments", &kAutoCollectAllFragments},
        {"kHideHudExceptForStarFragments", &kHideHudExceptForStarFragments},
        {"kAutoFragmentWarp", &kAutoFragmentWarp}, {"kFastBurn", &kFastBurn}, {"kDyeDebug", &kDyeDebug},
        {"kEnableAllRelationshipAbilities", &kEnableAllRelationshipAbilities},
        {"kFakeCapeLevelEnabled", &kFakeCapeLevelEnabled}, {"kFakeCapeLevel", &kFakeCapeLevel},
        {"kAllowOverride", &kAllowOverride}, {"kSunMoonXPosition", &kSunMoonXPosition},
        {"kSunMoonYPosition", &kSunMoonYPosition}, {"kSunMoon", &kSunMoon}, {"kMoonPhase", &kMoonPhase},
        {"kSunMoonSize", &kSunMoonSize}, {"kExposure", &kExposure},
        {"kFlameToCandleScale", &kFlameToCandleScale}, {"kFlowerHeight", &kFlowerHeight},
        {"kFlowerSize", &kFlowerSize}, {"kEnableGameCamSnap", &kEnableGameCamSnap},
        {"kAvatarCharcoaling", &kAvatarCharcoaling}, {"kAllNpcsHaveRadar", &kAllNpcsHaveRadar},
        {"kForceEthereal", &kForceEthereal}, {"kRevealPlayers", &kRevealPlayers},
        {"kEnableMultiplayer", &kEnableMultiplayer}, {"kDisableGates", &kDisableGates},
        {"kFastHome", &kFastHome}, {"kFreezeKrills", &kFreezeKrills}, {"kBirthdayKrills", &kBirthdayKrills},
        {"kTguiPauseAnimation", &kTguiPauseAnimation}, {"kUiShowHierarchy", &kUiShowHierarchy},
        {"kDebugShowSpiritLocations", &kDebugShowSpiritLocations},
        {"kShowRadarForPreviousWingBuffs", &kShowRadarForPreviousWingBuffs},
        {"kEnableShrineRadar", &kEnableShrineRadar}, {"kShowAllFeedback", &kShowAllFeedback},
        {"kMapShrineRadar", &kMapShrineRadar}, {"kFishSchoolDebug", &kFishSchoolDebug},
        {"kTvDebugUi", &kTvDebugUi}, {"kDisableWindWall", &kDisableWindWall},
        {"kDisableLevelChangeEvents", &kDisableLevelChangeEvents},
        {"kDisableObjectCollision", &kDisableObjectCollision}, {"kDisableAllCollision", &kDisableAllCollision},
        {"kDisableTerrain", &kDisableTerrain}, {"kDisableAvatars", &kDisableAvatars},
        {"kDisableObjects", &kDisableObjects}, {"kDisableObjectSkirts", &kDisableObjectSkirts},
        {"kDisableModels", &kDisableModels}, {"kEnableGravity", &kEnableGravity},
        {"kEnableClouds", &kEnableClouds}, {"kEnableWater", &kEnableWater}, {"kEnableOcean", &kEnableOcean},
        {"kDisableLights", &kDisableLights}, {"kAutoCompleteQuests", &kAutoCompleteQuests},
        {"kSuperLaunch", &kSuperLaunch}, {"kSpellEmitter", &kSpellEmitter}, {"kScooterMode", &kScooterMode},
        {"kRainbowGlow", &kRainbowGlow}, {"kBubbleTrails", &kBubbleTrails}, {"kRainbowTrails", &kRainbowTrails},
        {"kEnableReverb", &kEnableReverb}, {"kInstrumentAutoPlaySheets", &kInstrumentAutoPlaySheets},
        {"kInstrumentEasyMode", &kInstrumentEasyMode}, {"kInstrumentRadialLayout", &kInstrumentRadialLayout},
        {"kDisableRemoteOutfitCache", &kDisableRemoteOutfitCache}, {"kIOSHeadphones", &kIOSHeadphones},
        {"kFireworksCooldown", &kFireworksCooldown}, {"kFastFlap", &kFastFlap},
        {"kReadTableMessages", &kReadTableMessages}, {"kStarwatchAuth", &kStarwatchAuth},
        {"kAutoBurnPlants1", &kAutoBurnPlants1}, {"kAutoBurnPlants2", &kAutoBurnPlants2},
        {"kAutoBurnPlants3", &kAutoBurnPlants3}
    };
}

inline bool ParseOffsetValue(const nlohmann::json& value, std::uintptr_t& out) {
    try {
        unsigned long long parsed = 0;
        if (value.is_number_unsigned()) {
            parsed = value.get<unsigned long long>();
        } else if (value.is_number_integer()) {
            const long long signedValue = value.get<long long>();
            if (signedValue <= 0) return false;
            parsed = static_cast<unsigned long long>(signedValue);
        } else if (value.is_string()) {
            const std::string text = value.get<std::string>();
            if (text.empty()) return false;
            std::size_t consumed = 0;
            parsed = std::stoull(text, &consumed, 0);
            if (consumed != text.size()) return false;
        } else {
            return false;
        }
        if (parsed == 0 || parsed > kMaxAcceptedOffset) return false;
        out = static_cast<std::uintptr_t>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

inline const nlohmann::json* SelectVersionObject(const nlohmann::json& root) {
    if (root.is_object()) return &root;
    if (!root.is_array()) return nullptr;
    for (const auto& item : root) {
        if (item.is_object() && item.value("version", std::string()) == kSupportedVersion) return &item;
    }
    if (root.size() == 1 && root.front().is_object()) return &root.front();
    return nullptr;
}

inline bool LoadCanvasOffsets() {
    const std::string path = CanvasOffsetsPath();
    if (path.empty()) {
        tsm::log::w("RuntimeOffsets: Canvas config path unavailable; using compiled 0.34.5 defaults");
        return false;
    }

    std::ifstream input(path);
    if (!input) {
        tsm::log::i("RuntimeOffsets: %s not present; using compiled 0.34.5 defaults", path.c_str());
        return false;
    }

    try {
        nlohmann::json root;
        input >> root;
        const nlohmann::json* selected = SelectVersionObject(root);
        if (!selected) {
            tsm::log::e("RuntimeOffsets: invalid root or no matching 0.34.5 entry; ignoring file");
            return false;
        }

        const std::string version = selected->value("version", std::string(kSupportedVersion));
        if (version != kSupportedVersion) {
            tsm::log::e("RuntimeOffsets: version '%s' is incompatible with %s; ignoring file",
                        version.c_str(), kSupportedVersion);
            return false;
        }

        auto known = KnownOffsets();
        std::vector<std::pair<std::uintptr_t*, std::uintptr_t>> pending;
        pending.reserve(selected->size());

        for (auto it = selected->begin(); it != selected->end(); ++it) {
            const std::string& key = it.key();
            if (key == "version" || key == "contributors" || key == "comments") continue;
            auto target = known.find(key);
            if (target == known.end()) {
                tsm::log::w("RuntimeOffsets: unknown key '%s' ignored", key.c_str());
                continue;
            }
            std::uintptr_t parsed = 0;
            if (!ParseOffsetValue(it.value(), parsed)) {
                tsm::log::e("RuntimeOffsets: invalid value for '%s'; ignoring entire file", key.c_str());
                return false;
            }
            pending.emplace_back(target->second, parsed);
        }

        if (pending.empty()) {
            tsm::log::w("RuntimeOffsets: no recognized offsets in %s; compiled defaults retained", path.c_str());
            return false;
        }
        for (const auto& [target, value] : pending) *target = value;
        tsm::log::i("RuntimeOffsets: loaded %zu offsets for Sky %s from %s",
                    pending.size(), version.c_str(), path.c_str());
        return true;
    } catch (const std::exception& ex) {
        tsm::log::e("RuntimeOffsets: JSON parse/load failed (%s); compiled defaults retained", ex.what());
        return false;
    } catch (...) {
        tsm::log::e("RuntimeOffsets: JSON parse/load failed; compiled defaults retained");
        return false;
    }
}

} // namespace tsm::game::runtime_offsets
