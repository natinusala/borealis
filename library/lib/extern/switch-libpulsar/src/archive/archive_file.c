#include <pulsar/archive/archive_file.h>

PLSR_ArchiveFileHandle plsrArchiveFileOpen(const char* path) {
	FILE* f = fopen(path, "r");
	if(!f) {
		return PLSR_INVALID_ARCHIVE_FILE_HANDLE;
	}
	PLSR_ArchiveSharedReader* reader = (PLSR_ArchiveSharedReader*)malloc(sizeof(PLSR_ArchiveSharedReader));
	if(!reader) {
		fclose(f);
		return PLSR_INVALID_ARCHIVE_FILE_HANDLE;
	}

	reader->f = f;
	reader->refs = 1;

	return reader;
}

void plsrArchiveFileClose(PLSR_ArchiveFileHandle handle) {
	if(!handle) {
		return;
	}
	PLSR_ArchiveSharedReader* reader = (PLSR_ArchiveSharedReader*)handle;
	reader->refs--;
	if(reader->refs == 0) {
		fclose(reader->f);
		free(reader);
	}
}

PLSR_ArchiveFileHandle plsrArchiveFileCloneHandle(PLSR_ArchiveFileHandle handle) {
	if(handle) {
		PLSR_ArchiveSharedReader* reader = (PLSR_ArchiveSharedReader*)handle;
		reader->refs++;
	}
	return handle;
}
