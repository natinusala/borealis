#pragma once

#include <pulsar/types.h>

#define _PLSR_ARCHIVE_ENDIANNESS_LITLE 0xFEFF
#define _PLSR_ARCHIVE_ENDIANNESS_BIG 0xFFFF

typedef struct {
	char magic[4];
	u16 endianness;
	u16 headerSize;
	u32 version;
	u32 fileSize;
	u16 sectionCount;
	char _padX12[2];
} _PLSR_ArchiveHeader;

typedef struct {
	char magic[4];
	u32 size;
} _PLSR_ArchiveSectionHeader;

typedef struct {
	u32 count;
} _PLSR_ArchiveTableHeader;

typedef struct {
	u16 id;
	char _padX02[2];
	u32 offset;
} _PLSR_ArchiveRef;

typedef struct {
	u16 id;
	char _padX02[2];
	u32 offset;
	u32 size;
} _PLSR_ArchiveBlockRef;
