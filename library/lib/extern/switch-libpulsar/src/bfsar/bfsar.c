#include <pulsar/bfsar/bfsar.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFSAR, Init, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFSAR, Init, X)

static PLSR_RC _BFSARReadStringTreeHeaderInfo(PLSR_BFSAR* bfsar) {
	_PLSR_BFSARStringTreeHeader _header;
	_LOCAL_TRY(plsrArchiveReadAt(&bfsar->ar, bfsar->stringTree.offset, &_header, sizeof(_header)));

	bfsar->stringTree.info.rootNodeIndex = _header.rootNodeIndex;
	bfsar->stringTree.info.nodeCount = _header.nodeCount;

	return PLSR_RC_OK;
}

static PLSR_RC _BFSARReadHeaderStrg(PLSR_BFSAR* bfsar) {
	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfsar->ar, bfsar->strgSection.offset, _PLSR_BFSAR_STRG_MAGIC, &bfsar->strgSection.info));

	_PLSR_ArchiveRef _ref;
	unsigned identifierCount = 2;
	for(unsigned read = 0; (read < bfsar->strgSection.info.blockSize && identifierCount > 0); read += sizeof(_ref)) {
		_LOCAL_TRY(plsrArchiveRead(&bfsar->ar, &_ref, sizeof(_ref)));

		switch(_ref.id) {
			case _PLSR_BFSAR_STRG_IDENTIFIER_STRING_TABLE:
				bfsar->stringTable.offset = bfsar->strgSection.offset + sizeof(_PLSR_ArchiveSectionHeader) + _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFSAR_STRG_IDENTIFIER_STRING_TREE:
				bfsar->stringTree.offset = bfsar->strgSection.offset + sizeof(_PLSR_ArchiveSectionHeader) + _ref.offset;
				identifierCount--;
				break;
		}
	}

	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfsar->ar, bfsar->stringTable.offset, &bfsar->stringTable.info));
	_LOCAL_TRY(_BFSARReadStringTreeHeaderInfo(bfsar));

	return PLSR_RC_OK;
}

static PLSR_RC _BFSARReadHeaderInfo(PLSR_BFSAR* bfsar) {
	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfsar->ar, bfsar->infoSection.offset, _PLSR_BFSAR_INFO_MAGIC, &bfsar->infoSection.info));

	_PLSR_ArchiveRef _ref;
	unsigned identifierCount = 5;
	for(unsigned read = 0; read < bfsar->infoSection.info.blockSize && identifierCount > 0; read += sizeof(_ref)) {
		_LOCAL_TRY(plsrArchiveRead(&bfsar->ar, &_ref, sizeof(_ref)));

		switch(_ref.id) {
			case _PLSR_BFSAR_INFO_IDENTIFIER_SOUND_TABLE:
				bfsar->soundTable.offset = bfsar->infoSection.offset + sizeof(_PLSR_ArchiveSectionHeader) + _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFSAR_INFO_IDENTIFIER_WAVE_ARCHIVE_TABLE:
				bfsar->waveArchiveTable.offset = bfsar->infoSection.offset + sizeof(_PLSR_ArchiveSectionHeader) + _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFSAR_INFO_IDENTIFIER_GROUP_TABLE:
				bfsar->groupTable.offset = bfsar->infoSection.offset + sizeof(_PLSR_ArchiveSectionHeader) + _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFSAR_INFO_IDENTIFIER_FILE_TABLE:
				bfsar->fileTable.offset = bfsar->infoSection.offset + sizeof(_PLSR_ArchiveSectionHeader) + _ref.offset;
				identifierCount--;
				break;
		}
		// UNIMPLEMENTED: Remaining resource tables (bank, player, sar player)
	}

	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfsar->ar, bfsar->soundTable.offset, &bfsar->soundTable.info));
	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfsar->ar, bfsar->waveArchiveTable.offset, &bfsar->waveArchiveTable.info));
	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfsar->ar, bfsar->groupTable.offset, &bfsar->groupTable.info));
	_LOCAL_TRY(plsrArchiveReadTableHeaderInfo(&bfsar->ar, bfsar->fileTable.offset, &bfsar->fileTable.info));

	return PLSR_RC_OK;
}

static PLSR_RC _BFSARInit(PLSR_BFSAR* bfsar) {
	_LOCAL_TRY(plsrArchiveReadHeaderInfo(&bfsar->ar, _PLSR_BFSAR_MAGIC, &bfsar->headerInfo));

	_PLSR_ArchiveBlockRef _ref;
	unsigned identifierCount = 3;
	for(unsigned i = 0; i < bfsar->headerInfo.sectionCount && identifierCount > 0; i++) {
		_LOCAL_TRY(plsrArchiveRead(&bfsar->ar, &_ref, sizeof(_ref)));

		switch(_ref.id) {
			case _PLSR_BFSAR_SECTION_IDENTIFIER_STRG:
				bfsar->strgSection.offset = _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFSAR_SECTION_IDENTIFIER_INFO:
				bfsar->infoSection.offset = _ref.offset;
				identifierCount--;
				break;
			case _PLSR_BFSAR_SECTION_IDENTIFIER_FILE:
				bfsar->fileSection.offset = _ref.offset;
				identifierCount--;
				break;
		}
	}

	PLSR_RC_TRY(_BFSARReadHeaderStrg(bfsar));
	PLSR_RC_TRY(_BFSARReadHeaderInfo(bfsar));
	_LOCAL_TRY(plsrArchiveReadSectionHeaderInfo(&bfsar->ar, bfsar->fileSection.offset, _PLSR_BFSAR_FILE_MAGIC, &bfsar->fileSection.info));

	return PLSR_RC_OK;
}

PLSR_RC plsrBFSAROpen(const char* path, PLSR_BFSAR* out) {
	memset(out, 0, sizeof(PLSR_BFSAR));

	PLSR_RC rc = plsrArchiveOpen(path, &out->ar);

	if(PLSR_RC_SUCCEEDED(rc)) {
		rc = _BFSARInit(out);
	}

	if(PLSR_RC_FAILED(rc)) {
		plsrBFSARClose(out);
	}

	return rc;
}

void plsrBFSARClose(PLSR_BFSAR* bfwav) {
	plsrArchiveClose(&bfwav->ar);
}
