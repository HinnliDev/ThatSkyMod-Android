#!/usr/bin/env python3
from pathlib import Path
import json
import re
import sys

R = Path(__file__).resolve().parents[1]
errors = []

def fail(msg):
    errors.append(msg)

def read(rel):
    return (R / rel).read_text(encoding="utf-8")

# Canvas metadata: only fields supported by the current Canvas parser are emitted.
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

# Published Canvas offsets file contains only independently verified 0.34.5 values.
offsets_json = json.loads(read("config/offsets.json"))
if offsets_json.get("version") != "0.34.5":
    fail("offsets.json version is not 0.34.5")
expected_json = {
    "Game": 0x329BA20,
    "AudienceBarn": 0x3021B60,
    "CandleBarn": 0x3053298,
    "kGameInit": 0x172B17C,
    "kLuaDebugDoString": 0x28C03F4,
    "kLocalAvatarLocoUpdate": 0x11C3720,
    "kLuaState": 0x20,
    "kSystemAccountsAndroid": 0x2F859A8,
    "kAccountServerClient": 0x1C0,
    "kAvatarBarn": 0x310,
    "kWingBuffBarn": 0x660,
    "kCandleBarnField": 0x680,
    "kDarkstoneBarn": 0x6D0,
    "kRadianceBarnField": 0x6D8,
    "kLevelStateBarn": 0x898,
    "kLevelName": 0x948,
    "kLoginType": 0x1460,
    "kShouldRestart": 0x1464,
    "kLoadingType": 0x1470,
}
for key, expected in expected_json.items():
    value = offsets_json.get(key)
    try:
        actual = int(value, 0) if isinstance(value, str) else int(value)
    except (TypeError, ValueError):
        fail(f"offsets.json {key} is not a valid integer/hex string")
        continue
    if actual != expected:
        fail(f"offsets.json {key} != 0x{expected:X}")

# Compiled defaults must agree with the verified JSON and keep unverified code RVAs off.
offsets = read("include/game/memory/offsets.h")
def header_value(symbol):
    m = re.search(r"\b" + re.escape(symbol) + r"\b\s*=\s*(0[xX][0-9A-Fa-f]+|\d+)", offsets)
    return int(m.group(1), 0) if m else None

for key, expected in expected_json.items():
    if header_value(key) != expected:
        fail(f"compiled {key} != 0x{expected:X}")

unverified_native = [
    "kLuaPushLightUserData", "kWingBuffUpdate", "kRadianceBarn",
    "kAccountServerSetSession", "kHttpClientSetUserAgent", "kShouldDisplay", "kDoShout",
    "kSetJoinableFunction", "kPlayMusicKey", "kStopNote", "kPianoFrame",
    "kSharedMemoryPlayPianoSound", "kInvincibility", "kAutoCharge", "kDisableRainDrain",
    "kDarkCreatureTame", "kAllowAfk", "kRunSpeed", "kSuperSlidey",
    "kAutoCollectAllFragments", "kHideHudExceptForStarFragments", "kAutoFragmentWarp",
    "kFastBurn", "kDyeDebug", "kEnableAllRelationshipAbilities", "kFakeCapeLevelEnabled",
    "kFakeCapeLevel", "kAllowOverride", "kSunMoonXPosition", "kSunMoonYPosition",
    "kSunMoon", "kMoonPhase", "kSunMoonSize", "kExposure", "kFlameToCandleScale",
    "kFlowerHeight", "kFlowerSize", "kEnableGameCamSnap", "kAvatarCharcoaling",
    "kAllNpcsHaveRadar", "kForceEthereal", "kRevealPlayers", "kEnableMultiplayer",
    "kDisableGates", "kFastHome", "kFreezeKrills", "kBirthdayKrills",
    "kTguiPauseAnimation", "kUiShowHierarchy", "kDebugShowSpiritLocations",
    "kShowRadarForPreviousWingBuffs", "kEnableShrineRadar", "kShowAllFeedback",
    "kMapShrineRadar", "kFishSchoolDebug", "kTvDebugUi", "kDisableWindWall",
    "kDisableLevelChangeEvents", "kDisableObjectCollision", "kDisableAllCollision",
    "kDisableTerrain", "kDisableAvatars", "kDisableObjects", "kDisableObjectSkirts",
    "kDisableModels", "kEnableGravity", "kEnableClouds", "kEnableWater", "kEnableOcean",
    "kDisableLights", "kAutoCompleteQuests", "kSuperLaunch", "kSpellEmitter",
    "kScooterMode", "kRainbowGlow", "kBubbleTrails", "kRainbowTrails", "kEnableReverb",
    "kInstrumentAutoPlaySheets", "kInstrumentEasyMode", "kInstrumentRadialLayout",
    "kDisableRemoteOutfitCache", "kIOSHeadphones", "kFireworksCooldown", "kFastFlap",
    "kReadTableMessages", "kStarwatchAuth", "kAutoBurnPlants1", "kAutoBurnPlants2",
    "kAutoBurnPlants3",
]
for symbol in unverified_native:
    if header_value(symbol) != 0:
        fail(f"unverified compiled native RVA is enabled: {symbol}=0x{header_value(symbol):X}")
    if symbol in offsets_json:
        fail(f"unverified native RVA was published in offsets.json: {symbol}")

# The Lua queue must have a real verified recurring game-thread execution path.
lua_hook = read("src/game/hooks/LuaHook.cpp")
for needle in ("kLocalAvatarLocoUpdate", "LocalAvatarLocoUpdate_Hook", "ProcessNext", "WaxRunner::Get().Tick()"):
    if needle not in lua_hook:
        fail("Lua game-thread pump invariant missing: " + needle)
if "install_rva(\"LuaPushLightUserData\"" in lua_hook:
    fail("legacy LuaPushLightUserData hook is still active")

# Canvas runtime offsets must be loaded before Lua/native hooks initialize.
runtime = read("include/game/memory/RuntimeOffsets.h")
for needle in ("libTSM_offsets.json", "get_ConfigsPath", "0.34.5", "KnownOffsets", "ParseOffsetValue"):
    if needle not in runtime:
        fail("runtime offsets loader invariant missing: " + needle)
init = read("src/core/InitManager.cpp")
if "LoadCanvasOffsets()" not in init:
    fail("InitManager does not load Canvas offsets")
elif init.index("LoadCanvasOffsets()") > init.index("InitializeCore()"):
    fail("Canvas offsets are loaded too late")

# Unverified AOB patch paths that were known to be stale must not be active.
feature_manager = read("src/features/manager/FeatureManager.cpp")
if "CipherScanIdaPattern(kSuperFlight)" in feature_manager:
    fail("unverified SuperFlight signature is still active")
unlocks = read("src/features/unlocks/UnlockFeatures.cpp")
if "CreateNopPatchesForPattern" in unlocks:
    fail("unverified unlock AOB signatures are still active")

# All zero-RVA memory primitives must fail closed rather than translating zero to module base.
address_h = read("include/game/memory/Address.h")
if "rva == 0" not in address_h:
    fail("RvaToPtr does not reject RVA zero")
memory_cpp = read("src/game/memory/Memory.cpp")
if memory_cpp.count("rva == 0") < 5:
    fail("Memory direct RVA helpers do not consistently reject zero")

# Credits and new repository-local icon.
readme = read("README.md")
if "Developer: XeTrinityz" not in readme or "Contributors: Catyro, Hinnli" not in readme:
    fail("README credits are not in the requested Developer/Contributors format")
if "Developer / Original developer" in readme or "Co-authors / Contributors" in readme:
    fail("legacy slash-style README credits remain")
icon = (R / "resources/icons/icon.png")
if not icon.is_file() or icon.read_bytes()[:8] != b"\x89PNG\r\n\x1a\n":
    fail("resources/icons/icon.png is missing or is not a PNG")

# Updater must point only at the maintained repository.
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
