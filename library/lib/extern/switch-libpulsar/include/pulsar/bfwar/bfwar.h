/**
 * @file
 * @brief Wave archive init
 */
#pragma once

#include <pulsar/archive/archive.h>
#include <pulsar/bfwar/bfwar_internal.h>

/// Wave archive method categories
typedef enum {
	PLSR_BFWARCategoryType_Init = 0,
	PLSR_BFWARCategoryType_File,
} PLSR_BFWARCategoryType;

/// Wave archive file
typedef struct {
	PLSR_Archive ar;

	PLSR_ArchiveHeaderInfo headerInfo;

	PLSR_ArchiveSection infoSection;
	PLSR_ArchiveSection fileSection;

	PLSR_ArchiveTable fileTable;
} PLSR_BFWAR;

/// @copydoc plsrArchiveOpen
PLSR_RC plsrBFWAROpen(const char* path, PLSR_BFWAR* out);

/// @copydoc plsrArchiveOpenInside
PLSR_RC plsrBFWAROpenInside(const PLSR_Archive* ar, u32 offset, PLSR_BFWAR* out);

/// @copydoc plsrArchiveClose
void plsrBFWARClose(PLSR_BFWAR* bfwar);
