/**
 * @file
 * @brief Wave sound data wave id table access
 */
#pragma once

#include <pulsar/bfwsd/bfwsd.h>
#include <pulsar/bfsar/bfsar_item.h>

/// Wave sound data wave id
typedef struct {
	PLSR_BFSARItemId archiveItemId;
	u32 index;
} PLSR_BFWSDWaveId;

/// @copydoc plsrArchiveListReadEntries
PLSR_RC plsrBFWSDWaveIdListReadEntries(const PLSR_BFWSD* bfwsd, u32 startIndex, u32 count, PLSR_BFWSDWaveId* outIds, u32* outReadCount);

/// @copydoc plsrArchiveListGetEntry
PLSR_RC plsrBFWSDWaveIdListGetEntry(const PLSR_BFWSD* bfwsd, u32 index, PLSR_BFWSDWaveId* out);

NX_INLINE u32 plsrBFWSDWaveIdCount(const PLSR_BFWSD* bfwsd) {
	return bfwsd->waveIdList.info.count;
}
