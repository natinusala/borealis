/**
 * @file
 * @brief Sound archive init
 */
#pragma once

#include <pulsar/archive/archive.h>
#include <pulsar/bfsar/bfsar_internal.h>

/// Sound archive method categories
typedef enum {
	PLSR_BFSARCategoryType_Init = 0,
	PLSR_BFSARCategoryType_IdList,
	PLSR_BFSARCategoryType_String,
	PLSR_BFSARCategoryType_Sound,
	PLSR_BFSARCategoryType_WaveArchive,
	PLSR_BFSARCategoryType_Group,
	PLSR_BFSARCategoryType_File,
} PLSR_BFSARCategoryType;

/// String tree header information
typedef struct {
	u32 rootNodeIndex;
	u32 nodeCount;
} PLSR_BFSARStringTreeInfo;

/// String tree (patricia trie)
typedef struct {
	u32 offset; ///< Section start offset in sound archive
	PLSR_BFSARStringTreeInfo info; ///< Cached tree information
} PLSR_BFSARStringTree;

/// Sound archive file
typedef struct {
	PLSR_Archive ar;

	PLSR_ArchiveHeaderInfo headerInfo;

	PLSR_ArchiveSection strgSection;
	PLSR_ArchiveSection infoSection;
	PLSR_ArchiveSection fileSection;

	PLSR_ArchiveTable stringTable;
	PLSR_BFSARStringTree stringTree;

	PLSR_ArchiveTable soundTable;
	PLSR_ArchiveTable waveArchiveTable;
	PLSR_ArchiveTable groupTable;
	PLSR_ArchiveTable fileTable;
} PLSR_BFSAR;

/// @copydoc plsrArchiveOpen
PLSR_RC plsrBFSAROpen(const char* path, PLSR_BFSAR* out);

/// @copydoc plsrArchiveClose
void plsrBFSARClose(PLSR_BFSAR* bfsar);
