#include <pulsar/bfwav/bfwav.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFWAV, Init, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFWAV, Init, X)

static PLSR_RC _BFWAVInit(PLSR_BFWAV* bfwav) {
	_LOCAL_TRY(plsrArchiveReadHeaderInfo(&bfwav->ar, _PLSR_BFWAV_MAGIC, &bfwav->headerInfo));

	_PLSR_ArchiveBlockRef _ref;
	unsigned identifierCount = 2;
	for(unsigned i = 0; i < bfwav->headerInfo.sectionCount && identifierCount > 0; i++) {
		_LOCAL_TRY(plsrArchiveRead(&bfwav->ar, &_ref, sizeof(_ref)));

		switch(_ref.id) {
			case _PLSR_BFWAV_SECTION_IDENTIFIER_INFO:
				bfwav->infoSection.offset = _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFWAV_SECTION_IDENTIFIER_DATA:
				bfwav->dataSection.offset = _ref.offset;
				identifierCount--;
				break;
		}
	}

	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfwav->ar, bfwav->infoSection.offset, _PLSR_BFWAV_INFO_MAGIC, &bfwav->infoSection.info));
	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfwav->ar, bfwav->dataSection.offset, _PLSR_BFWAV_DATA_MAGIC, &bfwav->dataSection.info));

	return PLSR_RC_OK;
}

PLSR_RC plsrBFWAVOpen(const char* path, PLSR_BFWAV* out) {
	memset(out, 0, sizeof(PLSR_BFWAV));

	PLSR_RC rc = plsrArchiveOpen(path, &out->ar);

	if(PLSR_RC_SUCCEEDED(rc)) {
		rc = _BFWAVInit(out);
	}

	if(PLSR_RC_FAILED(rc)) {
		plsrBFWAVClose(out);
	}

	return rc;
}

PLSR_RC plsrBFWAVOpenInside(const PLSR_Archive* ar, u32 offset, PLSR_BFWAV* out) {
	memset(out, 0, sizeof(PLSR_BFWAV));

	PLSR_RC rc = plsrArchiveOpenInside(ar, offset, &out->ar);

	if(PLSR_RC_SUCCEEDED(rc)) {
		rc = _BFWAVInit(out);
	}

	if(PLSR_RC_FAILED(rc)) {
		plsrBFWAVClose(out);
	}

	return rc;
}

void plsrBFWAVClose(PLSR_BFWAV* bfwav) {
	plsrArchiveClose(&bfwav->ar);
}
