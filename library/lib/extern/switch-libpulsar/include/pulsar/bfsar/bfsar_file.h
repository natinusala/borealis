/**
 * @file
 * @brief Sound archive file table access
 */
#pragma once

#include <pulsar/bfsar/bfsar.h>

/// Sound archive file type
typedef enum {
	PLSR_BFSARFileInfoType_Unknown = 0, ///< Unknown

	/// Sub-file is embedded in the sound archive
	/** @note Can be embedded directly in the FILE section, or in one of the sound archive groups */
	PLSR_BFSARFileInfoType_Internal = _PLSR_BFSAR_FILE_IDENTIFIER_INTERNAL,

	/// Sub-file is external to the archive and referenced by path
	PLSR_BFSARFileInfoType_External = _PLSR_BFSAR_FILE_IDENTIFIER_EXTERNAL,
} PLSR_BFSARFileInfoType;

/// Sound archive external file location
typedef struct {
	char path[FS_MAX_PATH];
} PLSR_BFSARFileInfoExternal;

/// Sound archive internal file location
typedef struct {
	/// Can be `false` in case offset and/or size are not pointing to a file
	/**
	 * - `false` usually means the file is within a group (and that groups should be scanned to find the actual location)
	 * - Always `true` if returned from plsrBFSARFileScan()
	 */
	bool valid;

	u16 id;
	u32 offset;
	u32 size;
} PLSR_BFSARFileInfoInternal;

/// Sound archive file information
typedef struct {
	PLSR_BFSARFileInfoType type;
	union {
		PLSR_BFSARFileInfoInternal internal;
		PLSR_BFSARFileInfoExternal external;
	};
	bool fromGroup; ///< `true` if file was found in a group, as opposed to directly stored in the FILE section
	u32 groupIndex; ///< Group index the file was found in (populated if fromGroup is set to `true`)
} PLSR_BFSARFileInfo;

/// Fetch file information from the specified index in the file table
PLSR_RC plsrBFSARFileGet(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARFileInfo* out);

/// Similar to plsrBFSARFileGet(), but also scans groups for the requested index in case file is internal and has an invalid location
PLSR_RC plsrBFSARFileScan(const PLSR_BFSAR* bfsar, u32 index, PLSR_BFSARFileInfo* out);

NX_INLINE u32 plsrBFSARFileCount(const PLSR_BFSAR* bfsar) {
	return bfsar->fileTable.info.count;
}
