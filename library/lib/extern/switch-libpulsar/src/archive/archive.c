#include <pulsar/archive/archive.h>

PLSR_RC plsrArchiveOpen(const char* path, PLSR_Archive* out) {
	PLSR_ArchiveFileHandle handle = plsrArchiveFileOpen(path);
	if(handle == PLSR_INVALID_ARCHIVE_FILE_HANDLE) {
		return PLSR_ResultType_FileRead;
	}

	out->handle = handle;
	out->offset = 0;

	return PLSR_RC_OK;
}

PLSR_RC plsrArchiveOpenInside(const PLSR_Archive* ar, u32 offset, PLSR_Archive* out) {
	if(ar->handle == PLSR_INVALID_ARCHIVE_FILE_HANDLE) {
		return PLSR_ResultType_FileRead;
	}

	out->handle = plsrArchiveFileCloneHandle(ar->handle);
	out->offset = ar->offset + offset;

	return PLSR_RC_OK;
}

void plsrArchiveClose(PLSR_Archive* ar) {
	if(ar->handle) {
		plsrArchiveFileClose(ar->handle);
	}
	ar->handle = PLSR_INVALID_ARCHIVE_FILE_HANDLE;
}

PLSR_RC plsrArchiveReadAtEx(const PLSR_Archive* ar, u32 offset, void* out, size_t size, bool acceptZero) {
	if(
		(offset == 0 && !acceptZero)
		|| !plsrArchiveFileSetPosition(ar->handle, ar->offset + offset)
		|| !plsrArchiveFileRead(ar->handle, out, size)
	) {
		return PLSR_ResultType_FileRead;
	}

	return PLSR_RC_OK;
}

PLSR_RC plsrArchiveReadString(const PLSR_Archive* ar, u32 offset, char* out, size_t size) {
	if(
		offset == 0
		|| !plsrArchiveFileSetPosition(ar->handle, ar->offset + offset)
		|| !plsrArchiveFileReadString(ar->handle, out, size)
	) {
		return PLSR_ResultType_FileRead;
	}

	out[size-1] = '\0';
	return PLSR_RC_OK;
}

PLSR_RC plsrArchiveReadHeaderInfo(const PLSR_Archive* ar, const char* magic, PLSR_ArchiveHeaderInfo* out) {
	_PLSR_ArchiveHeader _header;

	PLSR_RC_TRY(plsrArchiveReadAtEx(ar, 0, &_header, sizeof(_header), true));

	if(memcmp(magic, &_header.magic, sizeof(_header.magic)) != 0) {
		return PLSR_ResultType_BadMagic;
	}

	if(_header.endianness != _PLSR_ARCHIVE_ENDIANNESS_LITLE) {
		return PLSR_ResultType_BadEndianness;
	}

	out->version.raw = _header.version;
	out->fileSize = _header.fileSize;
	out->sectionCount = _header.sectionCount;

	return PLSR_RC_OK;
}

PLSR_RC plsrArchiveReadSectionHeaderInfo(const PLSR_Archive* ar, u32 offset, const char* magic, PLSR_ArchiveSectionInfo* out) {
	_PLSR_ArchiveSectionHeader _header;

	PLSR_RC_TRY(plsrArchiveReadAt(ar, offset, &_header, sizeof(_header)));

	if(memcmp(magic, &_header.magic, sizeof(_header.magic)) != 0) {
		return PLSR_ResultType_BadMagic;
	}

	out->blockSize = _header.size;

	return PLSR_RC_OK;
}

PLSR_RC plsrArchiveReadTableHeaderInfo(const PLSR_Archive* ar, u32 offset, PLSR_ArchiveTableInfo* out) {
	_PLSR_ArchiveTableHeader _header;

	PLSR_RC_TRY(plsrArchiveReadAt(ar, offset, &_header, sizeof(_header)));

	out->count = _header.count;

	return PLSR_RC_OK;
}


PLSR_RC plsrArchiveReadTableEntry(const PLSR_Archive* ar, const PLSR_ArchiveTable* table, u16 id, u32 index, PLSR_ArchiveTableEntry* out) {
	if(table->offset == 0 || index >= table->info.count) {
		return PLSR_ResultType_NotFound;
	}

	_PLSR_ArchiveRef _ref;
	PLSR_RC_TRY(plsrArchiveReadAt(ar, table->offset + sizeof(_PLSR_ArchiveTableHeader) + index * sizeof(_ref), &_ref, sizeof(_ref)));

	if(_ref.id != id) {
		return PLSR_ResultType_NotFound;
	}

	out->offset = table->offset + _ref.offset;
	return PLSR_RC_OK;
}

PLSR_RC plsrArchiveReadTableBlock(const PLSR_Archive* ar, const PLSR_ArchiveTable* table, u16 id, u32 index, PLSR_ArchiveTableBlock* out) {
	if(table->offset == 0 || index >= table->info.count) {
		return PLSR_ResultType_NotFound;
	}

	_PLSR_ArchiveBlockRef _ref;
	PLSR_RC_TRY(plsrArchiveReadAt(ar, table->offset + sizeof(_PLSR_ArchiveTableHeader) + index * sizeof(_ref), &_ref, sizeof(_ref)));

	if(_ref.id != id) {
		return PLSR_ResultType_NotFound;
	}

	out->offset = table->offset + _ref.offset;
	out->size = _ref.size;

	return PLSR_RC_OK;
}

PLSR_RC plsrArchiveListReadEntries(const PLSR_Archive* ar, const PLSR_ArchiveList* list, u32 startIndex, u32 count, void* outEntries, u32* outReadCount) {
	u32 toRead = 0;
	if(startIndex < list->info.count) {
		u32 maxCount = list->info.count - startIndex;
		toRead = maxCount < count ? maxCount : count;
	}

	if(toRead > 0) {
		PLSR_RC_TRY(plsrArchiveReadAt(ar, list->offset + startIndex * list->info.entrySize, outEntries, toRead * list->info.entrySize));
	}

	if(outReadCount != NULL) {
		*outReadCount = toRead;
	}

	return PLSR_RC_OK;
}

PLSR_RC plsrArchiveListGetEntry(const PLSR_Archive* ar, const PLSR_ArchiveList* list, u32 index, void* out) {
	u32 read;
	PLSR_RC_TRY(plsrArchiveListReadEntries(ar, list, index, 1, out, &read));

	if(read == 0) {
		return PLSR_ResultType_NotFound;
	}

	return PLSR_RC_OK;
}
