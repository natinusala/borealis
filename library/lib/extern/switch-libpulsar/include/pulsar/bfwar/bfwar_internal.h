#pragma once

// @see https://github.com/Kinnay/Nintendo-File-Formats/wiki/BFWAR-File-Format

#include <pulsar/types.h>
#include <pulsar/archive/archive_internal.h>

#define _PLSR_BFWAR_MAGIC "FWAR"
#define _PLSR_BFWAR_INFO_MAGIC "INFO"
#define _PLSR_BFWAR_FILE_MAGIC "FILE"

#define _PLSR_BFWAR_SECTION_IDENTIFIER_INFO 0x6800
#define _PLSR_BFWAR_SECTION_IDENTIFIER_FILE 0x6801

#define _PLSR_BFWAR_FILE_IDENTIFIER_BLOCK_ENTRY 0x1F00

