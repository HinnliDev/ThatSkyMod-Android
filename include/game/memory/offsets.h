#pragma once

// Sky 0.34.5 verified anchors; unconfirmed module RVAs are 0 and fail closed.
// libBootloader.so SHA-256: 778b2e1d56c0fc2a47ee5b9db8b0384e65f70660dd4479a9578880c80958281f
// Runtime values are mutable so Canvas' local offsets JSON can safely override known keys.

#include <cstdint>
#include <utils/common/obfuscate.h>

namespace tsm { namespace game { namespace Offsets {

	inline std::uintptr_t Game = 0x329BA20;
	inline std::uintptr_t AudienceBarn = 0x3021B60;
	inline std::uintptr_t CandleBarn = 0x3053298;

	inline std::uintptr_t kLuaState = 0x20;
	inline std::uintptr_t kGameInit = 0x172B17C;
	inline std::uintptr_t kSystemAccountsAndroid = 0x2F859A8;
	inline std::uintptr_t kAccountServerClient = 0x1C0;
	inline std::uintptr_t kAccountBarnGameField = 0x1D8;
	inline std::uintptr_t kControllerGameField = 0x300;
	inline std::uintptr_t kWingBuffBarn = 0x660;
	inline std::uintptr_t kCandleBarnField = 0x680;
	inline std::uintptr_t kTimelineBarn = 0x6A0;
	inline std::uintptr_t kEventBarn = 0x6A8;
	inline std::uintptr_t kDarkstoneBarn = 0x6D0;
	inline std::uintptr_t kRadianceBarnField = 0x6D8;
	inline std::uintptr_t kDarkCreatureBarn = 0x6E8;
	inline std::uintptr_t kFriendBarn = 0x770;
	inline std::uintptr_t kFriendDataBarn = 0x778;
	inline std::uintptr_t kAffinityBarn = 0x780;
	inline std::uintptr_t kPickupEmitterBarn = 0x7C0;
	inline std::uintptr_t kPickupBarn = 0x7C8;
	inline std::uintptr_t kWaxChunkBarn = 0x7D0;
	inline std::uintptr_t kPickupNodeBarn = 0x7D8;
	inline std::uintptr_t kMusicBarn = 0x878;
	inline std::uintptr_t kSheetMusicBarn = 0x888;
	inline std::uintptr_t kLevelStateBarn = 0x898;
	inline std::uintptr_t kGameMode = 0x940;
	inline std::uintptr_t kServerHostname = 0x1350;
	inline std::uintptr_t kAvatarBarn = 0x310;
	inline std::uintptr_t kNetPlayerBarnPtr = 0x1C8;
	inline std::uintptr_t kNetPlayerBarnOffset = 0x20;

	inline std::uintptr_t kFirstPlayerIdOffset = 0x10;
	inline std::uintptr_t kFirstPlayerUuidOffset = 0x15;
	inline std::uintptr_t kPlayerIdStride = 0x5220;
	inline std::uintptr_t kAccountBarn = 0x8;
	inline std::uintptr_t kAvatarOutfit = 0x58;
	inline std::uintptr_t kAvatarLocalSlot = 0x30;
	inline std::uintptr_t kAvatarSlotStride = 0x10A30;
	inline std::uintptr_t kAvatarPosition = 0x18;
	inline std::uintptr_t kAvatarShout = 0x60;
	inline std::uintptr_t kLevelName = 0x948;

	inline std::uintptr_t kLoginType = 0x1460;
	inline std::uintptr_t& kAccountType = kLoginType;
	inline std::uintptr_t kLoadingType = 0x1470;
	inline std::uintptr_t& kAccountAuxState = kLoadingType;
	inline std::uintptr_t kShouldRestart = 0x1464;
	inline std::uintptr_t& kAccountSwitchRequested = kShouldRestart;

	inline std::uintptr_t kGameSpeedBarn = 0x30;
	inline std::uintptr_t kGameSpeedDelta = 0x28;
	inline std::uintptr_t kCameraSystem = 0x70;
	inline std::uintptr_t kCameraIntermediate = 0x158;
	inline std::uintptr_t kViewProjectionMatrix = 0x230;
	inline std::uintptr_t kJitterFullHalf = 0x60;
	inline std::uintptr_t kWhiskerCamera = 0x10D0;
	inline std::uintptr_t kCameraAngleX = 0x70C;
	inline std::uintptr_t kCameraAngleY = 0x710;
	inline std::uintptr_t kCameraRotation = 0x714;
	inline std::uintptr_t kCameraFOV = 0x6CC;
	inline std::uintptr_t kCameraZoom = 0x700;

	inline std::uintptr_t kHeight = 0x18DC;
	inline std::uintptr_t kScale = 0x18E0;
	inline std::uintptr_t kVoice = 0x1A50;
	inline std::uintptr_t kStance = 0x1A51;
	inline std::uintptr_t kBody = 0x144;
	inline std::uintptr_t kWing = 0x384;
	inline std::uintptr_t kHair = 0x5C4;
	inline std::uintptr_t kMask = 0x804;
	inline std::uintptr_t kNeck = 0xA44;
	inline std::uintptr_t kFeet = 0xC84;
	inline std::uintptr_t kHorn = 0xEC4;
	inline std::uintptr_t kFace = 0x1104;
	inline std::uintptr_t kProp = 0x1344;
	inline std::uintptr_t kHat = 0x1584;

	inline std::uintptr_t kLuaDebugDoString = 0x28C03F4;
	inline std::uintptr_t kLocalAvatarLocoUpdate = 0x11C3720;
	inline std::uintptr_t kLuaPushLightUserData = 0x0;
	inline std::uintptr_t kWingBuffUpdate = 0x0;
	inline std::uintptr_t kRadianceBarn = 0x0;
	inline std::uintptr_t kAccountServerSetSession = 0x0;
	inline std::uintptr_t kHttpClientSetUserAgent = 0x0;
	inline std::uintptr_t kShouldDisplay = 0x0;
	inline std::uintptr_t kDoShout = 0x0;
	inline std::uintptr_t kSetJoinableFunction = 0x0;
	inline std::uintptr_t kPlayMusicKey = 0x0;
	inline std::uintptr_t kStopNote = 0x0;
	inline std::uintptr_t kPianoFrame = 0x0;
	inline std::uintptr_t kSharedMemoryPlayPianoSound = 0x0;

	inline std::uintptr_t kInvincibility = 0x0;
	inline std::uintptr_t kAutoCharge = 0x0;
	inline std::uintptr_t kDisableRainDrain = 0x0;
	inline std::uintptr_t kDarkCreatureTame = 0x0;
	inline std::uintptr_t kAllowAfk = 0x0;
	inline std::uintptr_t kRunSpeed = 0x0;
	inline std::uintptr_t kSuperSlidey = 0x0;
	inline std::uintptr_t kAutoCollectAllFragments = 0x0;
	inline std::uintptr_t kHideHudExceptForStarFragments = 0x0;
	inline std::uintptr_t kAutoFragmentWarp = 0x0;
	inline std::uintptr_t kFastBurn = 0x0;
	inline std::uintptr_t kDyeDebug = 0x0;
	inline std::uintptr_t kEnableAllRelationshipAbilities = 0x0;
	inline std::uintptr_t kFakeCapeLevelEnabled = 0x0;
	inline std::uintptr_t kFakeCapeLevel = 0x0;
	inline std::uintptr_t kAllowOverride = 0x0;
	inline std::uintptr_t kSunMoonXPosition = 0x0;
	inline std::uintptr_t kSunMoonYPosition = 0x0;
	inline std::uintptr_t kSunMoon = 0x0;
	inline std::uintptr_t kMoonPhase = 0x0;
	inline std::uintptr_t kSunMoonSize = 0x0;
	inline std::uintptr_t kExposure = 0x0;
	inline std::uintptr_t kFlameToCandleScale = 0x0;
	inline std::uintptr_t kFlowerHeight = 0x0;
	inline std::uintptr_t kFlowerSize = 0x0;
	inline std::uintptr_t kEnableGameCamSnap = 0x0;
	inline std::uintptr_t kAvatarCharcoaling = 0x0;
	inline std::uintptr_t kAllNpcsHaveRadar = 0x0;
	inline std::uintptr_t kForceEthereal = 0x0;
	inline std::uintptr_t kRevealPlayers = 0x0;
	inline std::uintptr_t kEnableMultiplayer = 0x0;
	inline std::uintptr_t kDisableGates = 0x0;
	inline std::uintptr_t kFastHome = 0x0;
	inline std::uintptr_t kFreezeKrills = 0x0;
	inline std::uintptr_t kBirthdayKrills = 0x0;
	inline std::uintptr_t kTguiPauseAnimation = 0x0;
	inline std::uintptr_t kUiShowHierarchy = 0x0;
	inline std::uintptr_t kDebugShowSpiritLocations = 0x0;
	inline std::uintptr_t kShowRadarForPreviousWingBuffs = 0x0;
	inline std::uintptr_t kEnableShrineRadar = 0x0;
	inline std::uintptr_t kShowAllFeedback = 0x0;
	inline std::uintptr_t kMapShrineRadar = 0x0;
	inline std::uintptr_t kFishSchoolDebug = 0x0;
	inline std::uintptr_t kTvDebugUi = 0x0;
	inline std::uintptr_t kDisableWindWall = 0x0;
	inline std::uintptr_t kDisableLevelChangeEvents = 0x0;
	inline std::uintptr_t kDisableObjectCollision = 0x0;
	inline std::uintptr_t kDisableAllCollision = 0x0;
	inline std::uintptr_t kDisableTerrain = 0x0;
	inline std::uintptr_t kDisableAvatars = 0x0;
	inline std::uintptr_t kDisableObjects = 0x0;
	inline std::uintptr_t kDisableObjectSkirts = 0x0;
	inline std::uintptr_t kDisableModels = 0x0;
	inline std::uintptr_t kEnableGravity = 0x0;
	inline std::uintptr_t kEnableClouds = 0x0;
	inline std::uintptr_t kEnableWater = 0x0;
	inline std::uintptr_t kEnableOcean = 0x0;
	inline std::uintptr_t kDisableLights = 0x0;
	inline std::uintptr_t kAutoCompleteQuests = 0x0;
	inline std::uintptr_t kSuperLaunch = 0x0;
	inline std::uintptr_t kSpellEmitter = 0x0;
	inline std::uintptr_t kScooterMode = 0x0;
	inline std::uintptr_t kRainbowGlow = 0x0;
	inline std::uintptr_t kBubbleTrails = 0x0;
	inline std::uintptr_t kRainbowTrails = 0x0;
	inline std::uintptr_t kEnableReverb = 0x0;
	inline std::uintptr_t kInstrumentAutoPlaySheets = 0x0;
	inline std::uintptr_t kInstrumentEasyMode = 0x0;
	inline std::uintptr_t kInstrumentRadialLayout = 0x0;
	inline std::uintptr_t kDisableRemoteOutfitCache = 0x0;
	inline std::uintptr_t kIOSHeadphones = 0x0;
	inline std::uintptr_t kFireworksCooldown = 0x0;
	inline std::uintptr_t kFastFlap = 0x0;
	inline std::uintptr_t kReadTableMessages = 0x0;
	inline std::uintptr_t kStarwatchAuth = 0x0;
	inline std::uintptr_t kAutoBurnPlants1 = 0x0;
	inline std::uintptr_t kAutoBurnPlants2 = 0x0;
	inline std::uintptr_t kAutoBurnPlants3 = 0x0;
} } }

namespace tsm { namespace game { namespace Signatures {

	#define kUnlockAllPattern1 _O("?? 00 00 B4 20 00 80 52 F4 4F 42 A9 FD 7B 41 A9 FF C3 00 91 C0 03 5F D6 ?? ?? 43 F9")
	#define kUnlockAllPattern2 _O("?? ?? ?? ?? F8 03 00 AA ?? ?? 00 B5 E8 AA 4B 39")
	#define kEmoteLevelsPattern _O("48 2B 40 B9 ?? ?? 00 14")
	#define kLockCamPositionPattern _O("17 FD FF B5 E1 03 45 AD")
	#define kFreeZoomPattern _O("60 B2 05 BD 20 96 EC 97")
	#define kAutoCollectWaxPattern _O("E8 32 00 36 A9 26 00 90")
	#define kAutoBurnCandlesPattern _O("68 0C 00 34 60 42 41 BD")
	#define kRemoveCape _O("28 01 00 34 89 37 58 39")
	#define kSuperFlight _O("3F 01 00 B9 60 5A 4D BD")

	constexpr std::uint32_t kNopInstruction = 0xD503201Fu;
	constexpr std::uint32_t kRetInstruction = 0xD65F03C0u;
	constexpr std::uint32_t kMovW9Zero = 0x52800009u;
	constexpr std::uint32_t kMovW13Zero = 0x5280000Du;

} } }
