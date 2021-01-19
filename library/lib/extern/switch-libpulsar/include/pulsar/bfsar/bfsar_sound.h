/**
 * @file
 * @brief Sound archive sound table access
 */
#pragma once

#include <pulsar/bfsar/bfsar.h>
#include <pulsar/bfsar/bfsar_item.h>

/// Sound types
typedef enum {
	PLSR_BFSARSoundType_Stream = _PLSR_BFSAR_INFO_IDENTIFIER_STREAM_ENTRY,
	PLSR_BFSARSoundType_Wave = _PLSR_BFSAR_INFO_IDENTIFIER_WAVE_ENTRY,
	PLSR_BFSARSoundType_Sequence = _PLSR_BFSAR_INFO_IDENTIFIER_SEQUENCE_ENTRY,
} PLSR_BFSARSoundType;

/// Sound wave information
typedef struct {
	u32 index; ///< Wave index in wave archive
	u32 trackCount;
} PLSR_BFSARSoundWaveInfo;

/// Sound information
typedef struct {
	u32 fileIndex; ///< Related file index (wave type = WSD file, see plsrBFSARFileGet())
	PLSR_BFSARItemId playerItemId;
	u8 initialVolume;
	u8 remoteFilter;
	PLSR_BFSARSoundType type;

	PLSR_BFSARSoundWaveInfo wave; ///< Populated if type is `PLSR_BFSARSoundType_Wave`

	bool hasStringIndex;
	u32 stringIndex; ///< Name index in the string table, populated if `hasStringIndex` is `true` (see plsrBFSARStringGet())
} PLSR_BFSARSoundInfo;

/// Fetch sound information from the specified index in the sound table
PLSR_RC plsrBFSARSoundGet(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARSoundInfo* out);

NX_INLINE u32 plsrBFSARSoundCount(const PLSR_BFSAR* bfsar) {
	return bfsar->soundTable.info.count;
}
