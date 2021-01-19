#include <pulsar/bfgrp/bfgrp.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFGRP, Init, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFGRP, Init, X)

static PLSR_RC _BFGRPInit(PLSR_BFGRP* bfgrp) {
	_LOCAL_TRY(plsrArchiveReadHeaderInfo(&bfgrp->ar, _PLSR_BFGRP_MAGIC, &bfgrp->headerInfo));

	_PLSR_ArchiveBlockRef _ref;
	unsigned identifierCount = 2;
	for(unsigned i = 0; i < bfgrp->headerInfo.sectionCount && identifierCount > 0; i++) {
		_LOCAL_TRY(plsrArchiveRead(&bfgrp->ar, &_ref, sizeof(_ref)));

		switch(_ref.id) {
			case _PLSR_BFGRP_SECTION_IDENTIFIER_INFO:
				bfgrp->infoSection.offset = _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFGRP_SECTION_IDENTIFIER_FILE:
				bfgrp->fileSection.offset = _ref.offset;
				identifierCount--;
				break;
		}
	}

	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfgrp->ar, bfgrp->infoSection.offset, _PLSR_BFGRP_INFO_MAGIC, &bfgrp->infoSection.info));

	bfgrp->locationTable.offset = bfgrp->infoSection.offset + sizeof(_PLSR_ArchiveSectionHeader);
	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfgrp->ar, bfgrp->locationTable.offset, &bfgrp->locationTable.info));

	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfgrp->ar, bfgrp->fileSection.offset, _PLSR_BFGRP_FILE_MAGIC, &bfgrp->fileSection.info));

	return PLSR_RC_OK;
}

PLSR_RC plsrBFGRPOpen(const char* path, PLSR_BFGRP* out) {
	memset(out, 0, sizeof(PLSR_BFGRP));

	PLSR_RC rc = plsrArchiveOpen(path, &out->ar);

	if(PLSR_RC_SUCCEEDED(rc)) {
		rc = _BFGRPInit(out);
	}

	if(PLSR_RC_FAILED(rc)) {
		plsrBFGRPClose(out);
	}

	return rc;
}

PLSR_RC plsrBFGRPOpenInside(const PLSR_Archive* ar, u32 offset, PLSR_BFGRP* out) {
	memset(out, 0, sizeof(PLSR_BFGRP));

	PLSR_RC rc = plsrArchiveOpenInside(ar, offset, &out->ar);

	if(PLSR_RC_SUCCEEDED(rc)) {
		rc = _BFGRPInit(out);
	}

	if(PLSR_RC_FAILED(rc)) {
		plsrBFGRPClose(out);
	}

	return rc;
}

void plsrBFGRPClose(PLSR_BFGRP* bfgrp) {
	plsrArchiveClose(&bfgrp->ar);
}
