/**
 * @file
 * @brief Sound archive wave archive table access
 */
#pragma once

#include <pulsar/bfsar/bfsar.h>
#include <pulsar/bfwar/bfwar.h>

/// Wave archive information
typedef struct {
	u32 fileIndex; ///< Index in the file table (see plsrBFSARFileGet())

	bool hasStringIndex;
	u32 stringIndex; ///< Name index in the string table, populated if `hasStringIndex` is `true` (see plsrBFSARStringGet())

	bool hasWaveCount;
	u32 waveCount; ///< Total wave count in archive, populated if `hasWaveCount`
} PLSR_BFSARWaveArchiveInfo;

/// Fetch wave archive information from the specified index in the wave archive table
PLSR_RC plsrBFSARWaveArchiveGet(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARWaveArchiveInfo* out);

/// Open a wave archive from a plsrBFSARWaveArchiveGet() result (see plsrBFWAROpenInside())
PLSR_RC plsrBFSARWaveArchiveOpen(const PLSR_BFSAR* bfsar, const PLSR_BFSARWaveArchiveInfo* waveArchiveInfo, PLSR_BFWAR* out);

NX_INLINE u32 plsrBFSARWaveArchiveCount(const PLSR_BFSAR* bfsar) {
	return bfsar->waveArchiveTable.info.count;
}
