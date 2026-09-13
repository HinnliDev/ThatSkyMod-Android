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

// Strict whitelist: only values independently verified for the target 0.34.5 image
// can be overridden at runtime. Adding a key here requires REMCP verification first.
inline std::unordered_map<std::string, std::uintptr_t*> KnownOffsets() {
    using namespace tsm::game::Offsets;
    return {
        {"Game", &Game},
        {"kLuaState", &kLuaState},
        {"kGameInit", &kGameInit},
        {"kControllerGameField", &kControllerGameField},
        {"kAvatarBarn", &kAvatarBarn},
        {"kLevelName", &kLevelName},
        {"kLuaDebugDoString", &kLuaDebugDoString},
        {"kLocalAvatarLocoUpdate", &kLocalAvatarLocoUpdate},
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
            if (key == "version" || key == "targetSha256" || key == "contributors" || key == "comments") continue;
            auto target = known.find(key);
            if (target == known.end()) {
                tsm::log::w("RuntimeOffsets: unknown/unverified key '%s' ignored", key.c_str());
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
        tsm::log::i("RuntimeOffsets: loaded %zu verified offsets for Sky %s from %s",
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
