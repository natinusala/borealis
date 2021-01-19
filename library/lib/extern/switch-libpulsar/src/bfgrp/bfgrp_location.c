#include <pulsar/bfgrp/bfgrp_location.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFGRP, Location, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFGRP, Location, X)

static PLSR_RC _BFGRPLocationRead(const PLSR_BFGRP* bfgrp, u32 offset, const _PLSR_BFGRPLocationEntry* _locationEntry, PLSR_BFGRPLocationInfo* out) {
	out->fileIndex = _locationEntry->fileIndex;
	out->fileSize = _locationEntry->fileSize;

	if(_locationEntry->fileBlockRef.id != _PLSR_BFGRP_FILE_IDENTIFIER_BLOCK_ENTRY) {
		return _LOCAL_RC_MAKE(NotFound);
	}

	out->fileBlockOffset = bfgrp->fileSection.offset + sizeof(_PLSR_ArchiveSectionHeader) + _locationEntry->fileBlockRef.offset;

	return PLSR_RC_OK;
}

PLSR_RC plsrBFGRPLocationGet(const PLSR_BFGRP* bfgrp, u32 index, PLSR_BFGRPLocationInfo* out) {
	PLSR_ArchiveTableEntry tableEntry;

	_LOCAL_TRY(plsrArchiveReadTableEntry(&bfgrp->ar, &bfgrp->locationTable, _PLSR_BFGRP_INFO_IDENTIFIER_LOCATION_ENTRY, index, &tableEntry));

	_PLSR_BFGRPLocationEntry _locationEntry;
	_LOCAL_TRY(plsrArchiveReadAt(&bfgrp->ar, tableEntry.offset, &_locationEntry, sizeof(_locationEntry)));

	return _BFGRPLocationRead(bfgrp, tableEntry.offset, &_locationEntry, out);
}

PLSR_RC plsrBFGRPLocationFindByFileIndex(const PLSR_BFGRP* bfgrp, u32 fileIndex, PLSR_BFGRPLocationInfo* out) {
	PLSR_ArchiveTableEntry tableEntry;

	for(u32 i = 0; i < bfgrp->locationTable.info.count; i++) {
		_LOCAL_TRY(plsrArchiveReadTableEntry(&bfgrp->ar, &bfgrp->locationTable, _PLSR_BFGRP_INFO_IDENTIFIER_LOCATION_ENTRY, i, &tableEntry));

		_PLSR_BFGRPLocationEntry _locationEntry;
		_LOCAL_TRY(plsrArchiveReadAt(&bfgrp->ar, tableEntry.offset, &_locationEntry, sizeof(_locationEntry)));

		if(_locationEntry.fileIndex != fileIndex) {
			continue;
		}

		return _BFGRPLocationRead(bfgrp, tableEntry.offset, &_locationEntry, out);
	}

	return _LOCAL_RC_MAKE(NotFound);
}
