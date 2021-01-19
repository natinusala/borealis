/**
 * @file
 * @brief Wave sound data archive init
 */
#pragma once

#include <pulsar/archive/archive.h>
#include <pulsar/bfwsd/bfwsd_internal.h>

/// Wave sound data archive method categories
typedef enum {
	PLSR_BFWSDCategoryType_Init = 0,
	PLSR_BFWSDCategoryType_WaveId,
	PLSR_BFWSDCategoryType_SoundData,
} PLSR_BFWSDCategoryType;

/// Wave sound data archive
typedef struct {
	PLSR_Archive ar;

	PLSR_ArchiveHeaderInfo headerInfo;

	PLSR_ArchiveSection infoSection;

	PLSR_ArchiveList waveIdList;
	PLSR_ArchiveTable soundDataTable;
} PLSR_BFWSD;

/// @copydoc plsrArchiveOpen
PLSR_RC plsrBFWSDOpen(const char* path, PLSR_BFWSD* out);

/// @copydoc plsrArchiveOpenInside
PLSR_RC plsrBFWSDOpenInside(const PLSR_Archive* ar, u32 offset, PLSR_BFWSD* out);

/// @copydoc plsrArchiveClose
void plsrBFWSDClose(PLSR_BFWSD* bfwsd);
