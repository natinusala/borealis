#include <pulsar/bfwsd/bfwsd.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFWSD, Init, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFWSD, Init, X)

static PLSR_RC _BFWSDInit(PLSR_BFWSD* bfwsd) {
	_LOCAL_TRY(plsrArchiveReadHeaderInfo(&bfwsd->ar, _PLSR_BFWSD_MAGIC, &bfwsd->headerInfo));

	_PLSR_ArchiveBlockRef _blockRef;
	for(unsigned i = 0; i < bfwsd->headerInfo.sectionCount; i++) {
		_LOCAL_TRY(plsrArchiveRead(&bfwsd->ar, &_blockRef, sizeof(_blockRef)));

		if(_blockRef.id == _PLSR_BFWSD_SECTION_IDENTIFIER_INFO) {
			bfwsd->infoSection.offset = _blockRef.offset;
			break;
		}
	}

	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfwsd->ar, bfwsd->infoSection.offset, _PLSR_BFWSD_INFO_MAGIC, &bfwsd->infoSection.info));

	_PLSR_ArchiveRef _ref;
	unsigned identifierCount = 2;
	for(unsigned read = 0; read < bfwsd->infoSection.info.blockSize && identifierCount > 0; read += sizeof(_ref)) {
		_LOCAL_TRY(plsrArchiveRead(&bfwsd->ar, &_ref, sizeof(_ref)));

		switch(_ref.id) {
			case _PLSR_BFWSD_INFO_IDENTIFIER_WAVE_ID_TABLE:
				bfwsd->waveIdList.offset = bfwsd->infoSection.offset + sizeof(_PLSR_ArchiveSectionHeader) + _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFWSD_INFO_IDENTIFIER_SOUND_DATA_TABLE:
				bfwsd->soundDataTable.offset = bfwsd->infoSection.offset + sizeof(_PLSR_ArchiveSectionHeader) + _ref.offset;
				identifierCount--;
				break;
		}
	}

	_LOCAL_TRY(plsrArchiveReadAt(&bfwsd->ar, bfwsd->waveIdList.offset, &bfwsd->waveIdList.info.count, sizeof(bfwsd->waveIdList.info.count)));
	bfwsd->waveIdList.offset += sizeof(bfwsd->waveIdList.info.count);
	bfwsd->waveIdList.info.entrySize = sizeof(_PLSR_BFWSDWaveId);

	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfwsd->ar, bfwsd->soundDataTable.offset, &bfwsd->soundDataTable.info));

	return PLSR_RC_OK;
}

PLSR_RC plsrBFWSDOpen(const char* path, PLSR_BFWSD* out) {
	memset(out, 0, sizeof(PLSR_BFWSD));

	PLSR_RC rc = plsrArchiveOpen(path, &out->ar);

	if(PLSR_RC_SUCCEEDED(rc)) {
		rc = _BFWSDInit(out);
	}

	if(PLSR_RC_FAILED(rc)) {
		plsrBFWSDClose(out);
	}

	return rc;
}

PLSR_RC plsrBFWSDOpenInside(const PLSR_Archive* ar, u32 offset, PLSR_BFWSD* out) {
	memset(out, 0, sizeof(PLSR_BFWSD));

	PLSR_RC rc = plsrArchiveOpenInside(ar, offset, &out->ar);

	if(PLSR_RC_SUCCEEDED(rc)) {
		rc = _BFWSDInit(out);
	}

	if(PLSR_RC_FAILED(rc)) {
		plsrBFWSDClose(out);
	}

	return rc;
}

void plsrBFWSDClose(PLSR_BFWSD* bfwav) {
	plsrArchiveClose(&bfwav->ar);
}
