#pragma once

// @see https://github.com/Kinnay/Nintendo-File-Formats/wiki/BFGRP-File-Format

#include <pulsar/types.h>
#include <pulsar/archive/archive_internal.h>

#define _PLSR_BFGRP_MAGIC "FGRP"
#define _PLSR_BFGRP_INFO_MAGIC "INFO"
#define _PLSR_BFGRP_FILE_MAGIC "FILE"
#define _PLSR_BFGRP_INFO_EX_MAGIC "INFX"

#define _PLSR_BFGRP_SECTION_IDENTIFIER_INFO 0x7800
#define _PLSR_BFGRP_SECTION_IDENTIFIER_FILE 0x7801
#define _PLSR_BFGRP_SECTION_IDENTIFIER_INFO_EX 0x7802

#define _PLSR_BFGRP_INFO_IDENTIFIER_LOCATION_ENTRY 0x7900

#define _PLSR_BFGRP_FILE_IDENTIFIER_BLOCK_ENTRY 0x1f00

typedef struct {
	u32 fileIndex;
	_PLSR_ArchiveRef fileBlockRef;
	u32 fileSize;
} _PLSR_BFGRPLocationEntry;
