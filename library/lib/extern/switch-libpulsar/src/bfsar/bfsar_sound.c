#include <pulsar/bfsar/bfsar_sound.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFSAR, Sound, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFSAR, Sound, X)

static PLSR_RC _BFSARReadSoundWaveEntry(const PLSR_BFSAR* bfsar, u32 offset, PLSR_BFSARSoundWaveInfo* out) {
	_PLSR_BFSARSoundWaveEntry _waveEntry;
	_LOCAL_TRY(plsrArchiveReadAt(&bfsar->ar, offset, &_waveEntry, sizeof(_waveEntry)));

	out->index = _waveEntry.index;
	out->trackCount = _waveEntry.trackCount;

	return PLSR_RC_OK;
}

PLSR_RC plsrBFSARSoundGet(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARSoundInfo* out) {
	PLSR_ArchiveTableEntry tableEntry;

	_LOCAL_TRY(plsrArchiveReadTableEntry(&bfsar->ar, &bfsar->soundTable, _PLSR_BFSAR_INFO_IDENTIFIER_SOUND_ENTRY, index, &tableEntry));

	_PLSR_BFSARSoundEntry _soundEntry;
	_LOCAL_TRY(plsrArchiveReadAt(&bfsar->ar, tableEntry.offset, &_soundEntry, sizeof(_soundEntry)));

	out->fileIndex = _soundEntry.fileIndex;
	out->playerItemId.raw = _soundEntry.playerItemId;
	out->initialVolume = _soundEntry.initialVolume;
	out->remoteFilter = _soundEntry.remoteFilter;
	out->hasStringIndex = _soundEntry.flags & _PLSR_BFSAR_FLAG_STRING_INDEX;
	// UNIMPLEMENTED: Remaining flags

	if(out->hasStringIndex) {
		_LOCAL_TRY(plsrArchiveRead(&bfsar->ar, &out->stringIndex, sizeof(out->stringIndex)));
	}

	switch(_soundEntry.ref.id) {
		case _PLSR_BFSAR_INFO_IDENTIFIER_STREAM_ENTRY:
			out->type = PLSR_BFSARSoundType_Stream;
			// UNIMPLEMENTED: Stream specific infos
			break;
		case _PLSR_BFSAR_INFO_IDENTIFIER_WAVE_ENTRY:
			out->type = PLSR_BFSARSoundType_Wave;
			return _BFSARReadSoundWaveEntry(bfsar, tableEntry.offset + _soundEntry.ref.offset, &out->wave);
		case _PLSR_BFSAR_INFO_IDENTIFIER_SEQUENCE_ENTRY:
			out->type = PLSR_BFSARSoundType_Sequence;
			// UNIMPLEMENTED: Sequence specific infos
			break;
		default:
			return _LOCAL_RC_MAKE(Unsupported);
	}

	return PLSR_RC_OK;
}
