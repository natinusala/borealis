#include <pulsar/bfsar/bfsar_file.h>

#include <pulsar/bfsar/bfsar_group.h>
#include <pulsar/bfgrp/bfgrp.h>
#include <pulsar/bfgrp/bfgrp_location.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFSAR, File, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFSAR, File, X)

static PLSR_RC _BFSARFileReadInternal(const PLSR_BFSAR* bfsar, u32 offset, PLSR_BFSARFileInfoInternal* out) {
	_PLSR_ArchiveBlockRef _block;
	if(plsrArchiveReadAt(&bfsar->ar, offset, &_block, sizeof(_block))) {
		return _LOCAL_RC_MAKE(FileRead);
	}

	out->id = _block.id;
	out->offset = _block.offset;
	out->size = _block.size;
	out->valid = _block.id != 0xFFFF && _block.offset != 0xFFFFFFFF && _block.size != 0xFFFFFFFF;

	if(out->valid) {
		out->offset += bfsar->fileSection.offset + sizeof(_PLSR_ArchiveSectionHeader);
	}

	return PLSR_RC_OK;
}

static PLSR_RC _BFSARFileReadExternal(const PLSR_BFSAR* bfsar, u32 offset, PLSR_BFSARFileInfoExternal* out) {
	_LOCAL_TRY(plsrArchiveReadString(&bfsar->ar, offset, &out->path[0], sizeof(out->path)));

	return PLSR_RC_OK;
}

PLSR_RC plsrBFSARFileGet(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARFileInfo* out) {
	PLSR_ArchiveTableEntry tableEntry;
	_LOCAL_TRY(plsrArchiveReadTableEntry(&bfsar->ar, &bfsar->fileTable, _PLSR_BFSAR_INFO_IDENTIFIER_FILE_ENTRY, index, &tableEntry));

	_PLSR_ArchiveRef _ref;
	_LOCAL_TRY(plsrArchiveReadAt(&bfsar->ar, tableEntry.offset, &_ref, sizeof(_ref)));
	out->fromGroup = false;

	switch(_ref.id) {
		case _PLSR_BFSAR_FILE_IDENTIFIER_INTERNAL:
			out->type = PLSR_BFSARFileInfoType_Internal;
			return _BFSARFileReadInternal(bfsar, tableEntry.offset + _ref.offset, &out->internal);
		case _PLSR_BFSAR_FILE_IDENTIFIER_EXTERNAL:
			out->type = PLSR_BFSARFileInfoType_External;
			return _BFSARFileReadExternal(bfsar, tableEntry.offset + _ref.offset, &out->external);
		default:
			return _LOCAL_RC_MAKE(NotFound);
	}
}

PLSR_RC plsrBFSARFileScan(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARFileInfo* out) {
	PLSR_RC_TRY(plsrBFSARFileGet(bfsar, index, out));

	if(out->type != PLSR_BFSARFileInfoType_Internal || out->internal.valid) {
		return PLSR_RC_OK;
	}

	PLSR_BFSARGroupInfo groupInfo;
	PLSR_BFGRPLocationInfo locationInfo;
	PLSR_BFGRP bfgrp;
	for(u32 i = 0; i < plsrBFSARGroupCount(bfsar); i++) {
		PLSR_RC_TRY(plsrBFSARGroupGet(bfsar, i, &groupInfo));
		PLSR_RC_TRY(plsrBFSARGroupOpen(bfsar, &groupInfo, &bfgrp));

		PLSR_RC rc = plsrBFGRPLocationFindByFileIndex(&bfgrp, index, &locationInfo);
		plsrBFGRPClose(&bfgrp);

		if(PLSR_RC_RESULT(rc) == PLSR_ResultType_NotFound) {
			continue;
		} else if(PLSR_RC_FAILED(rc)) {
			return rc;
		}

		out->fromGroup = true;
		out->groupIndex = i;
		out->internal.offset = bfgrp.ar.offset + locationInfo.fileBlockOffset - bfsar->ar.offset;
		out->internal.size = locationInfo.fileSize;
		break;
	}

	return PLSR_RC_OK;
}
