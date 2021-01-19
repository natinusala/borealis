#include <pulsar/bfsar/bfsar_group.h>

#include <pulsar/bfsar/bfsar_file.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFSAR, Group, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFSAR, Group, X)

PLSR_RC plsrBFSARGroupGet(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARGroupInfo* out) {
	PLSR_ArchiveTableEntry tableEntry;

	_LOCAL_TRY(plsrArchiveReadTableEntry(&bfsar->ar, &bfsar->groupTable, _PLSR_BFSAR_INFO_IDENTIFIER_GROUP_ENTRY, index, &tableEntry));

	_PLSR_BFSARGroupEntry _groupEntry;
	_LOCAL_TRY(plsrArchiveReadAt(&bfsar->ar, tableEntry.offset, &_groupEntry, sizeof(_groupEntry)));

	out->fileIndex = _groupEntry.fileIndex;
	out->hasStringIndex = _groupEntry.flags & _PLSR_BFSAR_FLAG_STRING_INDEX;

	if(out->hasStringIndex) {
		_LOCAL_TRY(plsrArchiveRead(&bfsar->ar, &out->stringIndex, sizeof(out->stringIndex)));
	}

	return PLSR_RC_OK;
}

PLSR_RC plsrBFSARGroupOpen(const PLSR_BFSAR* bfsar, const PLSR_BFSARGroupInfo* groupInfo, PLSR_BFGRP* out) {
	PLSR_BFSARFileInfo fileInfo;
	PLSR_RC_TRY(plsrBFSARFileGet(bfsar, groupInfo->fileIndex, &fileInfo));

	switch(fileInfo.type) {
		case PLSR_BFSARFileInfoType_External:
			return plsrBFGRPOpen(fileInfo.external.path, out);
		case PLSR_BFSARFileInfoType_Internal:
			return plsrBFGRPOpenInside(&bfsar->ar, fileInfo.internal.offset, out);
		default:
			return _LOCAL_RC_MAKE(Unsupported);
	}
}
