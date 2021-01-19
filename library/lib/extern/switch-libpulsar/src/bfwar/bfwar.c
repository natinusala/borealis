#include <pulsar/bfwar/bfwar.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFWAR, Init, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFWAR, Init, X)

static PLSR_RC _BFWARInit(PLSR_BFWAR* bfwar) {
	_LOCAL_TRY(plsrArchiveReadHeaderInfo(&bfwar->ar, _PLSR_BFWAR_MAGIC, &bfwar->headerInfo));

	_PLSR_ArchiveBlockRef _ref;
	unsigned identifierCount = 2;
	for(unsigned i = 0; i < bfwar->headerInfo.sectionCount && identifierCount > 0; i++) {
		_LOCAL_TRY(plsrArchiveRead(&bfwar->ar, &_ref, sizeof(_ref)));

		switch(_ref.id) {
			case _PLSR_BFWAR_SECTION_IDENTIFIER_INFO:
				bfwar->infoSection.offset = _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFWAR_SECTION_IDENTIFIER_FILE:
				bfwar->fileSection.offset = _ref.offset;
				identifierCount--;
				break;
		}
	}

	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfwar->ar, bfwar->infoSection.offset, _PLSR_BFWAR_INFO_MAGIC, &bfwar->infoSection.info));
	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfwar->ar, bfwar->fileSection.offset, _PLSR_BFWAR_FILE_MAGIC, &bfwar->fileSection.info));

	bfwar->fileTable.offset = bfwar->infoSection.offset + sizeof(_PLSR_ArchiveSectionHeader);
	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfwar->ar, bfwar->fileTable.offset, &bfwar->fileTable.info));

	return PLSR_RC_OK;
}

PLSR_RC plsrBFWAROpen(const char* path, PLSR_BFWAR* out) {
	memset(out, 0, sizeof(PLSR_BFWAR));

	PLSR_RC rc = plsrArchiveOpen(path, &out->ar);

	if(PLSR_RC_SUCCEEDED(rc)) {
		rc = _BFWARInit(out);
	}

	if(PLSR_RC_FAILED(rc)) {
		plsrBFWARClose(out);
	}

	return rc;
}

PLSR_RC plsrBFWAROpenInside(const PLSR_Archive* ar, u32 offset, PLSR_BFWAR* out) {
	memset(out, 0, sizeof(PLSR_BFWAR));

	PLSR_RC rc = plsrArchiveOpenInside(ar, offset, &out->ar);

	if(PLSR_RC_SUCCEEDED(rc)) {
		rc = _BFWARInit(out);
	}

	if(PLSR_RC_FAILED(rc)) {
		plsrBFWARClose(out);
	}

	return rc;
}

void plsrBFWARClose(PLSR_BFWAR* bfwar) {
	plsrArchiveClose(&bfwar->ar);
}
