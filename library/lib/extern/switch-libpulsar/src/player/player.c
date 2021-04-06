#ifdef __SWITCH__

#include <pulsar/player/player.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(Player, Init, X)
#define _LOCAL_NX_TRY(X) PLSR_RC_NX_LTRY(Player, Init, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(Player, Init, X)

static const PLSR_PlayerConfig g_default_config = {
	.initRenderer = true,
	.audrenConfig = {
		.output_rate = AudioRendererOutputRate_48kHz,
		.num_voices = 24,
		.num_effects = 0,
		.num_sinks = 1,
		.num_mix_objs = 1,
		.num_mix_buffers = 2,
	},
	.startVoiceId = 0,
	.endVoiceId = 23,
	.sinkChannels = {0, 1}
};

static PLSR_Player* g_instance = NULL;

static PLSR_RC _playerCreate(PLSR_Player* out, const PLSR_PlayerConfig* config) {
	_LOCAL_NX_TRY(audrvCreate(&out->driver, &config->audrenConfig, PLSR_PLAYER_SINK_CHANNELS));

	memcpy(&out->config, config, sizeof(out->config));

	audrvDeviceSinkAdd(&out->driver, AUDREN_DEFAULT_DEVICE_NAME, PLSR_PLAYER_SINK_CHANNELS, config->sinkChannels);
	audrvUpdate(&out->driver);

	if(config->initRenderer) {
		audrenStartAudioRenderer();
	}

	return PLSR_RC_OK;
}

PLSR_Player* plsrPlayerGetInstance(void) {
	return g_instance;
}

const PLSR_PlayerConfig* plsrPlayerGetDefaultConfig(void) {
	return &g_default_config;
}

PLSR_RC plsrPlayerInitEx(const PLSR_PlayerConfig* config) {
	if(g_instance != NULL) {
		return PLSR_RC_OK;
	}

	if(config->startVoiceId < 0 || config->endVoiceId < 0
		|| config->startVoiceId > config->endVoiceId
		|| config->endVoiceId >= config->audrenConfig.num_voices) {
		return _LOCAL_RC_MAKE(BadInput);
	}

	if(config->initRenderer) {
		_LOCAL_NX_TRY(audrenInitialize(&config->audrenConfig));
	}

	g_instance = (PLSR_Player*)malloc(sizeof(PLSR_Player));
	if(g_instance == NULL) {
		return _LOCAL_RC_MAKE(Memory);
	}

	PLSR_RC rc = _playerCreate(g_instance, config);
	if(PLSR_RC_FAILED(rc)) {
		free(g_instance);
		g_instance = NULL;
		return rc;
	}

	return PLSR_RC_OK;
}

void plsrPlayerExit(void) {
	if(g_instance != NULL) {
		audrvClose(&g_instance->driver);

		if(g_instance->config.initRenderer) {
			audrenExit();
		}

		free(g_instance);
		g_instance = NULL;
	}
}

PLSR_RC plsrPlayerPlay(PLSR_PlayerSoundId id) {
	if(id == PLSR_PLAYER_INVALID_SOUND) {
		return _LOCAL_RC_MAKE(BadInput);
	}

	if(g_instance == NULL) {
		return _LOCAL_RC_MAKE(NotReady);
	}

	PLSR_PlayerSound* sound = (PLSR_PlayerSound*)id;
	for(unsigned int i = 0; i < sound->channelCount; i++) {
		audrvVoiceStop(&g_instance->driver, sound->channels[i].voiceId);
		audrvVoiceAddWaveBuf(&g_instance->driver, sound->channels[i].voiceId, &sound->channels[i].wavebuf);
		audrvVoiceStart(&g_instance->driver, sound->channels[i].voiceId);
	}

	audrvUpdate(&g_instance->driver);

	return PLSR_RC_OK;
}


PLSR_RC plsrPlayerStop(PLSR_PlayerSoundId id) {
	if(id == PLSR_PLAYER_INVALID_SOUND) {
		return _LOCAL_RC_MAKE(BadInput);
	}

	PLSR_PlayerSound* sound = (PLSR_PlayerSound*)id;
	for(unsigned int i = 0; i < sound->channelCount; i++) {
		if(sound->channels[i].voiceId != -1) {
			audrvVoiceStop(&g_instance->driver, sound->channels[i].voiceId);
		}
	}

	audrvUpdate(&g_instance->driver);

	return PLSR_RC_OK;
}

void plsrPlayerFree(PLSR_PlayerSoundId id) {
	if(id == PLSR_PLAYER_INVALID_SOUND) {
		return;
	}

	PLSR_PlayerSound* sound = (PLSR_PlayerSound*)id;
	for(unsigned int i = 0; i < sound->channelCount; i++) {
		if(sound->channels[i].voiceId != -1) {
			audrvVoiceDrop(&g_instance->driver, sound->channels[i].voiceId);
		}
		if(sound->channels[i].mempoolId != -1) {
			audrvMemPoolDetach(&g_instance->driver, sound->channels[i].mempoolId);
		}
	}

	audrvUpdate(&g_instance->driver);
	for(unsigned int i = 0; i < sound->channelCount; i++) {
		if(sound->channels[i].mempoolId != -1) {
			audrvMemPoolRemove(&g_instance->driver, sound->channels[i].mempoolId);
		}
		if(sound->channels[i].mempool != NULL) {
			free((void*)sound->channels[i].mempool);
		}
	}

	free(sound);
}

PLSR_RC plsrPlayerSetPitch(PLSR_PlayerSoundId id, float pitch) {
	if(id == PLSR_PLAYER_INVALID_SOUND) {
		return _LOCAL_RC_MAKE(BadInput);
	}

	PLSR_PlayerSound* sound = (PLSR_PlayerSound*)id;
	for(unsigned int i = 0; i < sound->channelCount; i++) {
		audrvVoiceSetPitch(&g_instance->driver, sound->channels[i].voiceId, pitch);
	}

	return PLSR_RC_OK;
}

PLSR_RC plsrPlayerSetVolume(PLSR_PlayerSoundId id, float volume) {
	if(id == PLSR_PLAYER_INVALID_SOUND) {
		return _LOCAL_RC_MAKE(BadInput);
	}

	PLSR_PlayerSound* sound = (PLSR_PlayerSound*)id;
	for(unsigned int i = 0; i < sound->channelCount; i++) {
		audrvVoiceSetVolume(&g_instance->driver, sound->channels[i].voiceId, volume);
	}

	return PLSR_RC_OK;
}

#endif
