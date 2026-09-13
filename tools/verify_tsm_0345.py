#!/usr/bin/env python3
from pathlib import Path
import json
import re
import sys

R = Path(__file__).resolve().parents[1]
errors = []
TARGET_SHA256 = "778b2e1d56c0fc2a47ee5b9db8b0384e65f70660dd4479a9578880c80958281f"

def fail(msg):
    errors.append(msg)

def read(rel):
    return (R / rel).read_text(encoding="utf-8")

cfg = json.loads(read("config/config.json"))
if (cfg.get("majorVersion"), cfg.get("minorVersion"), cfg.get("patchVersion")) != (0, 34, 5):
    fail("config semantic version is not 0.34.5")
if cfg.get("author") != "XeTrinityz, Catyro, Hinnli":
    fail("config author must be 'XeTrinityz, Catyro, Hinnli'")
for unsupported in ("contributors", "version", "repository"):
    if unsupported in cfg:
        fail(f"unsupported Canvas metadata key remains: {unsupported}")
if cfg.get("githubReleasesUrl") != "https://api.github.com/repos/HinnliDev/ThatSkyMod-Android/releases/latest":
    fail("githubReleasesUrl is not the HinnliDev latest-release API endpoint")
if cfg.get("offsetsUrl") != "https://raw.githubusercontent.com/HinnliDev/ThatSkyMod-Android/master/config/offsets.json":
    fail("offsetsUrl is not the stable raw master config/offsets.json URL")
if cfg.get("dependencies") != []:
    fail("dependencies must remain an array")

offsets_json = json.loads(read("config/offsets.json"))
if offsets_json.get("version") != "0.34.5":
    fail("offsets.json version is not 0.34.5")
if offsets_json.get("targetSha256") != TARGET_SHA256:
    fail("offsets.json targetSha256 does not match the REMCP target image")

verified = {
    "Game": 0x329BA20,
    "kLuaState": 0x20,
    "kGameInit": 0x172B17C,
    "kControllerGameField": 0x300,
    "kAvatarBarn": 0x310,
    "kLevelName": 0x948,
    "kLuaDebugDoString": 0x28C03F4,
    "kLocalAvatarLocoUpdate": 0x11C3720,
}
metadata_keys = {"version", "targetSha256", "contributors", "comments"}
for key in offsets_json:
    if key not in metadata_keys and key not in verified:
        fail(f"unverified key published in offsets.json: {key}")
for key, expected in verified.items():
    value = offsets_json.get(key)
    try:
        actual = int(value, 0) if isinstance(value, str) else int(value)
    except (TypeError, ValueError):
        fail(f"offsets.json {key} is not a valid integer/hex string")
        continue
    if actual != expected:
        fail(f"offsets.json {key} != 0x{expected:X}")

offsets = read("include/game/memory/offsets.h")
def header_value(symbol):
    m = re.search(r"\b" + re.escape(symbol) + r"\b\s*=\s*(0[xX][0-9A-Fa-f]+|\d+)", offsets)
    return int(m.group(1), 0) if m else None

for key, expected in verified.items():
    if header_value(key) != expected:
        fail(f"compiled {key} != 0x{expected:X}")

unverified_layout = [
    "AudienceBarn", "CandleBarn", "kSystemAccountsAndroid", "kAccountServerClient",
    "kAccountBarnGameField", "kWingBuffBarn", "kCandleBarnField", "kTimelineBarn", "kEventBarn",
    "kDarkstoneBarn", "kRadianceBarnField", "kDarkCreatureBarn", "kFriendBarn", "kFriendDataBarn",
    "kAffinityBarn", "kPickupEmitterBarn", "kPickupBarn", "kWaxChunkBarn", "kPickupNodeBarn",
    "kMusicBarn", "kSheetMusicBarn", "kLevelStateBarn", "kGameMode", "kServerHostname",
    "kNetPlayerBarnPtr", "kNetPlayerBarnOffset", "kFirstPlayerIdOffset", "kFirstPlayerUuidOffset",
    "kPlayerIdStride", "kAccountBarn", "kAvatarOutfit", "kAvatarLocalSlot", "kAvatarSlotStride",
    "kAvatarPosition", "kAvatarShout", "kLoginType", "kLoadingType", "kShouldRestart",
    "kGameSpeedBarn", "kGameSpeedDelta", "kCameraSystem", "kCameraIntermediate",
    "kViewProjectionMatrix", "kJitterFullHalf", "kWhiskerCamera", "kCameraAngleX", "kCameraAngleY",
    "kCameraRotation", "kCameraFOV", "kCameraZoom", "kHeight", "kScale", "kVoice", "kStance",
    "kBody", "kWing", "kHair", "kMask", "kNeck", "kFeet", "kHorn", "kFace", "kProp", "kHat",
]

unverified_native = [
    "kLuaPushLightUserData", "kWingBuffUpdate", "kRadianceBarn",
    "kAccountServerSetSession", "kHttpClientSetUserAgent", "kShouldDisplay", "kDoShout",
    "kSetJoinableFunction", "kPlayMusicKey", "kStopNote", "kPianoFrame",
    "kSharedMemoryPlayPianoSound", "kInvincibility", "kAutoCharge", "kDisableRainDrain",
    "kDarkCreatureTame", "kAllowAfk", "kRunSpeed", "kSuperSlidey",
    "kAutoCollectAllFragments", "kHideHudExceptForStarFragments", "kAutoFragmentWarp",
    "kFastBurn", "kDyeDebug", "kEnableAllRelationshipAbilities", "kFakeCapeLevelEnabled",
    "kFakeCapeLevel", "kAllowOverride", "kSunMoonXPosition", "kSunMoonYPosition", "kSunMoon",
    "kMoonPhase", "kSunMoonSize", "kExposure", "kFlameToCandleScale", "kFlowerHeight",
    "kFlowerSize", "kEnableGameCamSnap", "kAvatarCharcoaling", "kAllNpcsHaveRadar",
    "kForceEthereal", "kRevealPlayers", "kEnableMultiplayer", "kDisableGates", "kFastHome",
    "kFreezeKrills", "kBirthdayKrills", "kTguiPauseAnimation", "kUiShowHierarchy",
    "kDebugShowSpiritLocations", "kShowRadarForPreviousWingBuffs", "kEnableShrineRadar",
    "kShowAllFeedback", "kMapShrineRadar", "kFishSchoolDebug", "kTvDebugUi", "kDisableWindWall",
    "kDisableLevelChangeEvents", "kDisableObjectCollision", "kDisableAllCollision", "kDisableTerrain",
    "kDisableAvatars", "kDisableObjects", "kDisableObjectSkirts", "kDisableModels", "kEnableGravity",
    "kEnableClouds", "kEnableWater", "kEnableOcean", "kDisableLights", "kAutoCompleteQuests",
    "kSuperLaunch", "kSpellEmitter", "kScooterMode", "kRainbowGlow", "kBubbleTrails",
    "kRainbowTrails", "kEnableReverb", "kInstrumentAutoPlaySheets", "kInstrumentEasyMode",
    "kInstrumentRadialLayout", "kDisableRemoteOutfitCache", "kIOSHeadphones", "kFireworksCooldown",
    "kFastFlap", "kReadTableMessages", "kStarwatchAuth", "kAutoBurnPlants1", "kAutoBurnPlants2",
    "kAutoBurnPlants3",
]
for symbol in unverified_layout + unverified_native:
    value = header_value(symbol)
    if value is None:
        fail(f"offset symbol missing from header: {symbol}")
    elif value != 0:
        fail(f"unverified compiled offset is enabled: {symbol}=0x{value:X}")
    if symbol in offsets_json:
        fail(f"unverified offset was published in offsets.json: {symbol}")

runtime = read("include/game/memory/RuntimeOffsets.h")
for needle in ("libTSM_offsets.json", "get_ConfigsPath", "0.34.5", "KnownOffsets", "ParseOffsetValue", "targetSha256"):
    if needle not in runtime:
        fail("runtime offsets loader invariant missing: " + needle)
for symbol in unverified_layout + unverified_native:
    if re.search(r'\{"' + re.escape(symbol) + r'"\s*,', runtime):
        fail("unverified offset is runtime-overridable: " + symbol)

lua_hook = read("src/game/hooks/LuaHook.cpp")
for needle in ("kLocalAvatarLocoUpdate", "LocalAvatarLocoUpdate_Hook", "ProcessNext", "WaxRunner::Get().Tick()"):
    if needle not in lua_hook:
        fail("Lua game-thread pump invariant missing: " + needle)
if "install_rva(\"LuaPushLightUserData\"" in lua_hook:
    fail("legacy LuaPushLightUserData hook is still active")

lua_bridge = read("src/game/interop/lua_bridge.cpp")
if "rva == 0" not in lua_bridge or "base == 0" not in lua_bridge:
    fail("lua_bridge does not reject zero base/RVA before arithmetic")

init = read("src/core/InitManager.cpp")
if "LoadCanvasOffsets()" not in init:
    fail("InitManager does not load Canvas offsets")
elif init.index("LoadCanvasOffsets()") > init.index("InitializeCore()"):
    fail("Canvas offsets are loaded too late")
if "PerformEarlyInit()" in init and "InstallCameraHook()" in init.split("void InitManager::PerformLateInit()", 1)[0]:
    fail("native camera hook is installed before runtime offsets load")

patch_manager = read("src/progression/PatchManager.cpp")
auto_wax = read("src/progression/AutoWaxTools.cpp")
for stale in ("CreateNopPatchesForPattern(kAutoCollectWaxPattern", "CreateNopPatchesForPattern(kAutoBurnCandlesPattern"):
    if stale in patch_manager or stale in auto_wax:
        fail("stale 0.34.5 AOB scanner path remains reachable: " + stale)

feature_manager = read("src/features/manager/FeatureManager.cpp")
if "CipherScanIdaPattern(kSuperFlight)" in feature_manager:
    fail("unverified SuperFlight signature is still active")
unlocks = read("src/features/unlocks/UnlockFeatures.cpp")
if "CreateNopPatchesForPattern" in unlocks:
    fail("unverified unlock AOB signatures are still active")

address_h = read("include/game/memory/Address.h")
if "rva == 0" not in address_h:
    fail("RvaToPtr does not reject RVA zero")
mem_h = read("include/game/memory/mem.h")
if "off == 0" not in mem_h:
    fail("mem::add does not reject struct offset zero")
memory_cpp = read("src/game/memory/Memory.cpp")
if memory_cpp.count("rva == 0") < 5:
    fail("Memory direct RVA helpers do not consistently reject zero")

api_cpp = read("src/game/memory/api.cpp")
for needle in ("kAvatarLocalSlot == 0", "kNetPlayerBarnPtr == 0", "kCameraSystem == 0", "CandleBarn == 0"):
    if needle not in api_cpp:
        fail("api fail-closed invariant missing: " + needle)
shout = read("src/game/hooks/ShoutHook.cpp")
if "kDoShout == 0" not in shout or "kAvatarShout == 0" not in shout:
    fail("ShoutHook is not fail-closed for unverified RVA/layout")

readme = read("README.md")
if "Developer: XeTrinityz" not in readme or "Contributors: Catyro, Hinnli" not in readme:
    fail("README credits are not in the requested Developer/Contributors format")
icon = (R / "resources/icons/icon.png")
if not icon.is_file() or icon.read_bytes()[:8] != b"\x89PNG\r\n\x1a\n":
    fail("resources/icons/icon.png is missing or is not a PNG")

updater = read("src/core/UpdateChecker.cpp")
for needle in ("/repos/HinnliDev/ThatSkyMod-Android/releases/latest", "ThatSkyMod-Android-arm64-v8a.so", "Version{ 0, 34, 5 }"):
    if needle not in updater:
        fail("updater invariant missing: " + needle)

old_repo = "XeTrinityz/" + "ThatSkyMod-Android"
for p in R.rglob("*"):
    if not p.is_file() or ".git" in p.parts:
        continue
    if p.suffix.lower() not in {".cpp", ".cc", ".c", ".h", ".hpp", ".json", ".md", ".yml", ".yaml", ".py", ".txt"}:
        continue
    try:
        text = p.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        continue
    if old_repo in text:
        fail("legacy repository URL remains: " + str(p.relative_to(R)))

if errors:
    print("TSM 0.34.5 preflight FAILED")
    for error in errors:
        print(" -", error)
    sys.exit(1)
print("TSM 0.34.5 preflight OK")
