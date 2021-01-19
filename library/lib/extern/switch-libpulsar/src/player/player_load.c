#ifdef __SWITCH__

#include <pulsar/player/player_load.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(Player, Load, X)
#define _LOCAL_NX_TRY(X) PLSR_RC_NX_LTRY(Player, Load, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(Player, Load, X)
#define _AUDREN_ALIGN(sz) ((sz + (AUDREN_MEMPOOL_ALIGNMENT-1)) &~ (AUDREN_MEMPOOL_ALIGNMENT-1))

static int _playerGetFreeVoiceId(const PLSR_Player* player) {
	for(int id = player->config.startVoiceId; id <= player->config.endVoiceId; id++) {
		if(!player->driver.in_voices[id].is_used) {
			return id;
		}
	}

	return -1;
}

static PLSR_RC _playerLoadWave(PLSR_Player* player, const PLSR_BFWAV* bfwav, PLSR_PlayerSoundId* out) {
	PLSR_BFWAVInfo waveInfo;

	PLSR_RC_TRY(plsrBFWAVReadInfo(bfwav, &waveInfo));

	PcmFormat pcmFormat;
	size_t dataSize;
	switch(waveInfo.format) {
		case PLSR_BFWAVFormat_PCM_8:
			pcmFormat = PcmFormat_Int8;
			dataSize = waveInfo.sampleCount;
			break;
		case PLSR_BFWAVFormat_PCM_16:
			pcmFormat = PcmFormat_Int16;
			dataSize = waveInfo.sampleCount * 2;
			break;
		case PLSR_BFWAVFormat_DSP_ADPCM:
			pcmFormat = PcmFormat_Adpcm;
			dataSize = waveInfo.sampleCount * 8 + 1;
			dataSize = dataSize / 14 + dataSize % 14;
			break;
		default:
			return _LOCAL_RC_MAKE(Unsupported);
	}

	PLSR_PlayerSound* sound = (PLSR_PlayerSound*)malloc(sizeof(PLSR_PlayerSound));
	if(sound == NULL) {
		return _LOCAL_RC_MAKE(Memory);
	}
	*out = sound;

	sound->channelCount = 0;
	for(unsigned int channel = 0; channel < waveInfo.channelInfoTable.info.count; channel++) {
		sound->channelCount++;

		memset(&sound->channels[channel].wavebuf, 0, sizeof(AudioDriverWaveBuf));
		sound->channels[channel].mempool = NULL;
		sound->channels[channel].mempoolId = -1;
		sound->channels[channel].voiceId = _playerGetFreeVoiceId(player);

		if(sound->channels[channel].voiceId == -1) {
			return _LOCAL_RC_MAKE(Memory);
		}

		if(!audrvVoiceInit(&player->driver, sound->channels[channel].voiceId, 1, pcmFormat, waveInfo.sampleRate)) {
			return _LOCAL_RC_MAKE(System);
		}

		audrvVoiceSetDestinationMix(&player->driver, sound->channels[channel].voiceId, AUDREN_FINAL_MIX_ID);

		if(waveInfo.channelInfoTable.info.count == 1) {
			for(unsigned int i = 0; i < PLSR_PLAYER_SINK_CHANNELS; i++) {
				audrvVoiceSetMixFactor(&player->driver, sound->channels[channel].voiceId, 0.5f, 0, player->config.sinkChannels[i]);
			}
		} else if(sound->channelCount <= PLSR_PLAYER_SINK_CHANNELS) {
			audrvVoiceSetMixFactor(&player->driver, sound->channels[channel].voiceId, 0.5f, 0, player->config.sinkChannels[sound->channelCount-1]);
		} else {
			return _LOCAL_RC_MAKE(Unsupported);
		}

		size_t alignedDataSize = _AUDREN_ALIGN(dataSize);
		size_t alignedAdpcmParametersSize = waveInfo.format == PLSR_BFWAVFormat_DSP_ADPCM ? _AUDREN_ALIGN(sizeof(AudioRendererAdpcmParameters)) : 0;
		size_t alignedAdpcmContextSize = waveInfo.format == PLSR_BFWAVFormat_DSP_ADPCM ? _AUDREN_ALIGN(sizeof(AudioRendererAdpcmContext)) : 0;
		size_t mempoolSize = alignedDataSize + alignedAdpcmParametersSize + alignedAdpcmContextSize;

		sound->channels[channel].mempool = memalign(AUDREN_MEMPOOL_ALIGNMENT, mempoolSize);

		if(sound->channels[channel].mempool == NULL) {
			return _LOCAL_RC_MAKE(Memory);
		}

		void* dataPtr = sound->channels[channel].mempool;
		AudioRendererAdpcmParameters* adpcmParameters = (AudioRendererAdpcmParameters*)(dataPtr + alignedDataSize);
		AudioRendererAdpcmContext* adpcmContext = (AudioRendererAdpcmContext*)(dataPtr + alignedDataSize + alignedAdpcmParametersSize);

		sound->channels[channel].wavebuf.data_raw = dataPtr;
		sound->channels[channel].wavebuf.size = dataSize;
		sound->channels[channel].wavebuf.end_sample_offset = waveInfo.sampleCount;

		PLSR_BFWAVChannelInfo channelInfo;
		_LOCAL_TRY(plsrBFWAVReadChannelInfo(bfwav, &waveInfo.channelInfoTable, channel, &channelInfo));

		if(waveInfo.format == PLSR_BFWAVFormat_DSP_ADPCM) {
			// TODO: static assert context libnx = plsr adpcm context
			adpcmContext->index = channelInfo.adpcmInfo.loop.header;
			adpcmContext->history0 = channelInfo.adpcmInfo.loop.yn1;
			adpcmContext->history1 = channelInfo.adpcmInfo.loop.yn2;
			sound->channels[channel].wavebuf.context_addr = adpcmContext;
			sound->channels[channel].wavebuf.context_sz = sizeof(AudioRendererAdpcmContext);

			memcpy(adpcmParameters, &channelInfo.adpcmInfo.coeffs[0], sizeof(AudioRendererAdpcmParameters));
			audrvVoiceSetExtraParams(&player->driver, sound->channels[channel].voiceId, adpcmParameters, sizeof(AudioRendererAdpcmParameters));
		}

		_LOCAL_TRY(plsrArchiveReadAt(&bfwav->ar, channelInfo.dataOffset, dataPtr, dataSize));

		armDCacheFlush(sound->channels[channel].mempool, mempoolSize);
		sound->channels[channel].mempoolId = audrvMemPoolAdd(&player->driver, sound->channels[channel].mempool, mempoolSize);
		audrvMemPoolAttach(&player->driver, sound->channels[channel].mempoolId);
		audrvVoiceStart(&player->driver, sound->channels[channel].mempoolId);
		audrvUpdate(&player->driver);
	}

	return PLSR_RC_OK;
}

PLSR_RC plsrPlayerLoadWave(const PLSR_BFWAV* bfwav, PLSR_PlayerSoundId* out) {
	PLSR_Player* player = plsrPlayerGetInstance();
	if(player == NULL) {
		return _LOCAL_RC_MAKE(NotReady);
	}

	PLSR_PlayerSoundId id = NULL;
	PLSR_RC rc = _playerLoadWave(player, bfwav, &id);

	if(PLSR_RC_SUCCEEDED(rc)) {
		*out = id;
	} else {
		*out = NULL;
		plsrPlayerFree(id);
	}

	return rc;
}

#endif
