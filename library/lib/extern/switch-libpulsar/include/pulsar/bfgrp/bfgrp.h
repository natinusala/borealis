/**
 * @file
 * @brief Group init
 */
#pragma once

#include <pulsar/archive/archive.h>
#include <pulsar/bfgrp/bfgrp_internal.h>

/// Group method categories
typedef enum {
	PLSR_BFGRPCategoryType_Init = 0,
	PLSR_BFGRPCategoryType_Location,
} PLSR_BFGRPCategoryType;

/// Group file
typedef struct {
	PLSR_Archive ar;

	PLSR_ArchiveHeaderInfo headerInfo;

	PLSR_ArchiveSection infoSection;
	PLSR_ArchiveSection fileSection;

	PLSR_ArchiveTable locationTable;
} PLSR_BFGRP;

/// @copydoc plsrArchiveOpen
PLSR_RC plsrBFGRPOpen(const char* path, PLSR_BFGRP* out);

/// @copydoc plsrArchiveOpenInside
PLSR_RC plsrBFGRPOpenInside(const PLSR_Archive* ar, u32 offset, PLSR_BFGRP* out);

/// @copydoc plsrArchiveClose
void plsrBFGRPClose(PLSR_BFGRP* bfgrp);
