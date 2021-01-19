#include <pulsar/bfsar/bfsar_wave_archive.h>

#include <pulsar/bfsar/bfsar_file.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFSAR, WaveArchive, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFSAR, WaveArchive, X)

PLSR_RC plsrBFSARWaveArchiveGet(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARWaveArchiveInfo* out) {
	PLSR_ArchiveTableEntry tableEntry;

	_LOCAL_TRY(plsrArchiveReadTableEntry(&bfsar->ar, &bfsar->waveArchiveTable, _PLSR_BFSAR_INFO_IDENTIFIER_WAVE_ARCHIVE_ENTRY, index, &tableEntry));

	_PLSR_BFSARWaveArchiveEntry _waveArchiveEntry;
	_LOCAL_TRY(plsrArchiveReadAt(&bfsar->ar, tableEntry.offset, &_waveArchiveEntry, sizeof(_waveArchiveEntry)));

	out->fileIndex = _waveArchiveEntry.fileIndex;
	out->hasStringIndex = _waveArchiveEntry.flags & _PLSR_BFSAR_FLAG_STRING_INDEX;
	out->hasWaveCount = _waveArchiveEntry.flags & _PLSR_BFSAR_FLAG_WAVE_ARCHIVE_COUNT;

	if(out->hasStringIndex) {
		_LOCAL_TRY(plsrArchiveRead(&bfsar->ar, &out->stringIndex, sizeof(out->stringIndex)));
	}

	if(out->hasWaveCount) {
		_LOCAL_TRY(plsrArchiveRead(&bfsar->ar, &out->waveCount, sizeof(out->waveCount)));
	}

	return PLSR_RC_OK;
}

PLSR_RC plsrBFSARWaveArchiveOpen(const PLSR_BFSAR* bfsar, const PLSR_BFSARWaveArchiveInfo* waveArchiveInfo, PLSR_BFWAR* out) {
	PLSR_BFSARFileInfo fileInfo;
	PLSR_RC_TRY(plsrBFSARFileScan(bfsar, waveArchiveInfo->fileIndex, &fileInfo));

	switch(fileInfo.type) {
		case PLSR_BFSARFileInfoType_External:
			return plsrBFWAROpen(fileInfo.external.path, out);
		case PLSR_BFSARFileInfoType_Internal:
			return plsrBFWAROpenInside(&bfsar->ar, fileInfo.internal.offset, out);
		default:
			return _LOCAL_RC_MAKE(Unsupported);
	}
}
