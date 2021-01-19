/**
 * @file
 * @brief Group location table access
 */
#pragma once

#include <pulsar/bfgrp/bfgrp.h>

/// Group file location information
typedef struct {
	u32 fileIndex;
	u32 fileBlockOffset;
	u32 fileSize;
} PLSR_BFGRPLocationInfo;

/// Fetch location information from the specified index in the location table
PLSR_RC plsrBFGRPLocationGet(const PLSR_BFGRP* bfgrp, u32 index, PLSR_BFGRPLocationInfo* out);
/// Fetch location information from the specified index in the location table
PLSR_RC plsrBFGRPLocationGet(const PLSR_BFGRP* bfgrp, u32 index, PLSR_BFGRPLocationInfo* out);

/// Find location information matching the specified fileIndex
PLSR_RC plsrBFGRPLocationFindByFileIndex(const PLSR_BFGRP* bfgrp, u32 fileIndex, PLSR_BFGRPLocationInfo* out);

NX_INLINE u32 plsrBFGRPLocationCount(const PLSR_BFGRP* bfgrp) {
	return bfgrp->locationTable.info.count;
}
