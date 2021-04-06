/**
 * @file
 * @brief Player init
 */
#pragma once

#include <pulsar/archive/archive.h>
#include <pulsar/bfwav/bfwav.h>
#include <pulsar/bfwav/bfwav_info.h>

#define PLSR_PLAYER_SINK_CHANNELS 2
#define PLSR_PLAYER_INVALID_SOUND NULL

/// Player method categories
typedef enum {
	PLSR_PlayerCategoryType_Init = 0,
	PLSR_PlayerCategoryType_Load,
	PLSR_PlayerCategoryType_LoadLookup,
} PLSR_PlayerCategoryType;

/// Player config
typedef struct {
	bool initRenderer; ///< `true` if audren service init should be handled by the player

	AudioRendererConfig audrenConfig; ///< Audio renderer config (output rate, voices count, ...)
	int startVoiceId; ///< First renderer voice index to be used by the player (should be `>= 0 && < audrenConfig.num_voices`)
	int endVoiceId; ///< Last renderer voice index to be used by the player (should be `>= 0 && <= startVoiceId`)
	const u8 sinkChannels[PLSR_PLAYER_SINK_CHANNELS]; ///< Sink channels the player should use
} PLSR_PlayerConfig;

/// Player sound channel
typedef struct {
	void* mempool; ///< Pointer to the aligned memory containing audio samples (and adpcm parameters when applicable)
	AudioDriverWaveBuf wavebuf; ///< Audio driver audio buffer struct
	int mempoolId; ///< Audio driver assigned mempool index
	int voiceId; ///< Audio driver assigned voice index
} PLSR_PlayerSoundChannel;

/// Player sound
typedef struct {
	unsigned int channelCount;
	PLSR_PlayerSoundChannel channels[PLSR_PLAYER_SINK_CHANNELS];
} PLSR_PlayerSound;

/// Player sound ID
typedef const PLSR_PlayerSound* PLSR_PlayerSoundId;

/// Player
typedef struct {
	AudioDriver driver; ///< Audio driver internal state
	PLSR_PlayerConfig config; ///< Effective player configuration
} PLSR_Player;

/// Get default player configuration
const PLSR_PlayerConfig* plsrPlayerGetDefaultConfig(void);

/// Get player instance (NULL if not initialized)
PLSR_Player* plsrPlayerGetInstance(void);

/// Initialize player with a custom configuration
PLSR_RC plsrPlayerInitEx(const PLSR_PlayerConfig* config);

/// Initialize player with the default configuration
NX_INLINE PLSR_RC plsrPlayerInit(void) {
	return plsrPlayerInitEx(plsrPlayerGetDefaultConfig());
}

/// Convenience shortcut to wait until next audio renderer frame
NX_INLINE void plsrPlayerWaitNextFrame(void) {
	audrenWaitFrame();
}

/// De-initialize player
void plsrPlayerExit(void);

/// Play a loaded sound from the beginning
PLSR_RC plsrPlayerPlay(PLSR_PlayerSoundId id);

/// Stop a sound if it is currently playing
PLSR_RC plsrPlayerStop(PLSR_PlayerSoundId id);

/// Free ressources used by a loaded sound
void plsrPlayerFree(PLSR_PlayerSoundId id);

/// Set sound pitch factor (effective next time it's played)
PLSR_RC plsrPlayerSetPitch(PLSR_PlayerSoundId id, float pitch);

/// Set sound volume factor (effective next time it's played)
PLSR_RC plsrPlayerSetVolume(PLSR_PlayerSoundId id, float volume);
