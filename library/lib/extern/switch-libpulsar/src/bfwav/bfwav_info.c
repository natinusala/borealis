#include <pulsar/bfwav/bfwav_info.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFWAV, Info, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFWAV, Info, X)

PLSR_RC plsrBFWAVReadInfo(const PLSR_BFWAV* bfwav, PLSR_BFWAVInfo* out) {
	if(bfwav->infoSection.info.blockSize < sizeof(_PLSR_BFWAVInfo)) {
		return _LOCAL_RC_MAKE(NotFound);
	}

	_PLSR_BFWAVInfo _info;
	u32 infoOffset = bfwav->infoSection.offset + sizeof(_PLSR_ArchiveSectionHeader);
	_LOCAL_TRY(plsrArchiveReadAt(&bfwav->ar, infoOffset, &_info, sizeof(_info)));

	out->format = _info.format;
	out->looping = _info.looping;
	out->sampleRate = _info.sampleRate;
	out->loopStartSample = _info.loopStartSample;
	out->sampleCount = _info.sampleCount;

	out->channelInfoTable.offset = infoOffset + sizeof(_PLSR_BFWAVInfo);
	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfwav->ar, out->channelInfoTable.offset, &out->channelInfoTable.info));

	return PLSR_RC_OK;
}

PLSR_RC plsrBFWAVReadChannelInfo(const PLSR_BFWAV* bfwav, const PLSR_BFWAVChannelInfoTable* table, u32 index, PLSR_BFWAVChannelInfo* out) {
	PLSR_ArchiveTableEntry tableEntry;
	_LOCAL_TRY(plsrArchiveReadTableEntry(&bfwav->ar, table, _PLSR_BFWAV_INFO_IDENTIFIER_CHANNEL_ENTRY, index, &tableEntry));

	_PLSR_BFWAVChannelInfo _channelInfo;
	_LOCAL_TRY(plsrArchiveReadAt(&bfwav->ar, tableEntry.offset, &_channelInfo, sizeof(_channelInfo)));

	if(_channelInfo.dataRef.id != _PLSR_BFWAV_INFO_IDENTIFIER_DATA_ENTRY) {
		return _LOCAL_RC_MAKE(NotFound);
	}

	out->dataOffset = _channelInfo.dataRef.offset + bfwav->dataSection.offset + sizeof(_PLSR_ArchiveSectionHeader);

	if(_channelInfo.adpcmInfoRef.id == _PLSR_BFWAV_INFO_IDENTIFIER_ADPCM_ENTRY) {
		_LOCAL_TRY(plsrArchiveReadAt(&bfwav->ar, tableEntry.offset + _channelInfo.adpcmInfoRef.offset, &out->adpcmInfo, sizeof(out->adpcmInfo)));
	}

	return PLSR_RC_OK;
}
