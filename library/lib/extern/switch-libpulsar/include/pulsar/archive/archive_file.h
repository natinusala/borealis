/**
 * @file
 * @brief Shared archive file handle management
 * @note These should not be needed directly, use the wrapped archive functions instead
 */
#pragma once

#include <pulsar/types.h>

#define PLSR_INVALID_ARCHIVE_FILE_HANDLE NULL

typedef struct {
	FILE* f;
	size_t refs;
} PLSR_ArchiveSharedReader;

typedef const PLSR_ArchiveSharedReader* PLSR_ArchiveFileHandle;

PLSR_ArchiveFileHandle plsrArchiveFileOpen(const char* path);
void plsrArchiveFileClose(PLSR_ArchiveFileHandle handle);
PLSR_ArchiveFileHandle plsrArchiveFileCloneHandle(PLSR_ArchiveFileHandle handle);

NX_INLINE bool plsrArchiveFileRead(PLSR_ArchiveFileHandle handle, void* out, size_t size) {
	return handle && fread(out, size, 1, handle->f) == 1;
}

NX_INLINE bool plsrArchiveFileSetPosition(PLSR_ArchiveFileHandle handle, long offset) {
	return handle && fseek(handle->f, offset, SEEK_SET) != -1;
}

NX_INLINE bool plsrArchiveFileReadString(PLSR_ArchiveFileHandle handle, char* out, size_t size) {
	return handle && fgets(out, size, handle->f) != NULL;
}
