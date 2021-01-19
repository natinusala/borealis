#include <pulsar/bfwsd/bfwsd_sound_data.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFWSD, SoundData, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFWSD, SoundData, X)

PLSR_RC _BFWSDReadSoundInfo(const PLSR_BFWSD* bfwsd, u32 offset, PLSR_BFWSDSoundDataInfo* out) {
	_PLSR_BFWSDSoundInfoEntry _soundInfoEntry;
	u32 tmp;
	u32 sendOffset = 0;
	u32 adshrOffset = 0;

	_LOCAL_TRY(plsrArchiveReadAt(&bfwsd->ar, offset, &_soundInfoEntry, sizeof(_soundInfoEntry)));

	out->hasPan = _soundInfoEntry.flags & _PLSR_BFWSD_FLAG_SOUND_INFO_PAN;
	out->hasPitch = _soundInfoEntry.flags & _PLSR_BFWSD_FLAG_SOUND_INFO_PITCH;

	if(out->hasPan) {
		_LOCAL_TRY(plsrArchiveRead(&bfwsd->ar, &tmp, sizeof(tmp)));
		out->pan = tmp & 0xFF;
		out->surroundPan = (tmp >> 8) & 0xFF;
	}

	if(out->hasPitch) {
		_LOCAL_TRY(plsrArchiveRead(&bfwsd->ar, &out->pitch, sizeof(out->pitch)));
	}

	if(_soundInfoEntry.flags & _PLSR_BFWSD_FLAG_SOUND_INFO_SEND) {
		_LOCAL_TRY(plsrArchiveRead(&bfwsd->ar, &sendOffset, sizeof(sendOffset)));
	}

	if(_soundInfoEntry.flags & _PLSR_BFWSD_FLAG_SOUND_INFO_ADSHR) {
		_LOCAL_TRY(plsrArchiveRead(&bfwsd->ar, &adshrOffset, sizeof(adshrOffset)));
	}

	out->hasSend = sendOffset != 0 && sendOffset != 0xFFFFFFFF;
	out->hasAdshr = adshrOffset != 0 && adshrOffset != 0xFFFFFFFF;

	if(out->hasSend) {
		// While we could read main + count first, and then read the rest using the count we just got,
		// one read is enough, and should always work because of the padding to 8 bytes
		_LOCAL_TRY(plsrArchiveReadAt(&bfwsd->ar, sendOffset + offset, &out->send, sizeof(out->send)));

		if(out->send.auxCount > (sizeof(out->send.aux) / sizeof(out->send.aux[0]))) {
			return _LOCAL_RC_MAKE(Unsupported);
		}
	}

	if(out->hasAdshr) {
		_LOCAL_TRY(plsrArchiveReadAt(&bfwsd->ar, adshrOffset + offset, &out->adshr, sizeof(out->adshr)));
	}

	return PLSR_RC_OK;
}

PLSR_RC plsrBFWSDSoundDataGet(const PLSR_BFWSD* bfwsd, u32 index, PLSR_BFWSDSoundDataInfo* out) {
	PLSR_ArchiveTableEntry tableEntry;
	_LOCAL_TRY(plsrArchiveReadTableEntry(&bfwsd->ar, &bfwsd->soundDataTable, _PLSR_BFWSD_INFO_IDENTIFIER_SOUND_DATA_ENTRY, index, &tableEntry));

	_PLSR_BFWSDSoundDataEntry _soundDataEntry;
	_LOCAL_TRY(plsrArchiveReadAt(&bfwsd->ar, tableEntry.offset, &_soundDataEntry, sizeof(_soundDataEntry)));

	if(_soundDataEntry.noteInfoTableRef.id != _PLSR_BFWSD_INFO_IDENTIFIER_NOTE_TABLE) {
		return _LOCAL_RC_MAKE(NotFound);
	}

	out->noteInfoTable.offset = tableEntry.offset + _soundDataEntry.noteInfoTableRef.offset;
	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfwsd->ar, out->noteInfoTable.offset, &out->noteInfoTable.info));

	if(_soundDataEntry.soundInfoRef.id != _PLSR_BFWSD_INFO_IDENTIFIER_SOUND_INFO_ENTRY) {
		return _LOCAL_RC_MAKE(NotFound);
	}
	PLSR_RC_TRY(_BFWSDReadSoundInfo(bfwsd, _soundDataEntry.soundInfoRef.offset, out));
	// UNIMPLEMENTED: Track Info Reference

	return PLSR_RC_OK;
}

PLSR_RC plsrBFWSDSoundDataNoteGet(const PLSR_BFWSD* bfwsd, const PLSR_BFWSDNoteInfoTable* table, u32 index, PLSR_BFWSDNoteInfo* out) {
	PLSR_ArchiveTableEntry tableEntry;
	_LOCAL_TRY(plsrArchiveReadTableEntry(&bfwsd->ar, table, _PLSR_BFWSD_INFO_IDENTIFIER_NOTE_ENTRY, index, &tableEntry));

	_PLSR_BFWSDNoteEntry _noteEntry;
	_LOCAL_TRY(plsrArchiveReadAt(&bfwsd->ar, tableEntry.offset, &_noteEntry, sizeof(_noteEntry)));

	out->waveIdIndex = _noteEntry.waveIdIndex;
	// UNIMPLEMENTED: flags

	return PLSR_RC_OK;
}
