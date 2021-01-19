/**
 * @file
 * @brief Wave archive init
 */
#pragma once

#include <pulsar/bfwar/bfwar.h>

/// Wave archive file information
typedef struct {
	u32 offset;
	u32 size;
} PLSR_BFWARFileInfo;

/// Fetch wave file location information from the specified index in the file table
PLSR_RC plsrBFWARFileGet(const PLSR_BFWAR* bfwar, u32 index, PLSR_BFWARFileInfo* out);

NX_INLINE u32 plsrBFWARFileCount(const PLSR_BFWAR* bfwar) {
	return bfwar->fileTable.info.count;
}
