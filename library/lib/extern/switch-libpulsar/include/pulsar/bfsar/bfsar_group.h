/**
 * @file
 * @brief Sound archive group table access
 */
#pragma once

#include <pulsar/bfsar/bfsar.h>
#include <pulsar/bfgrp/bfgrp.h>

/// Sound archive group information
typedef struct {
	u32 fileIndex; ///< Index in the file table (see plsrBFSARFileGet())

	bool hasStringIndex;
	u32 stringIndex; ///< Name index in the string table, populated if `hasStringIndex` is `true` (see plsrBFSARStringGet())
} PLSR_BFSARGroupInfo;

/// Fetch group information from the specified index in the group table
PLSR_RC plsrBFSARGroupGet(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARGroupInfo* out);

/// Open a group file from a plsrBFSARGroupGet() result (see plsrBFGRPOpenInside())
PLSR_RC plsrBFSARGroupOpen(const PLSR_BFSAR* bfsar, const PLSR_BFSARGroupInfo* groupInfo, PLSR_BFGRP* out);

NX_INLINE u32 plsrBFSARGroupCount(const PLSR_BFSAR* bfsar) {
	return bfsar->groupTable.info.count;
}
