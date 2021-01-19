/**
 * @file
 * @brief Archive container interface
 */
#pragma once

#include <pulsar/types.h>
#include <pulsar/archive/archive_internal.h>
#include <pulsar/archive/archive_file.h>

/// Archive version found in header
typedef union {
	u32 raw;
	struct {
		u8 revision;
		u8 minor;
		u8 major;
	};
} PLSR_ArchiveVersion;

/// Common archive header information
typedef struct {
	PLSR_ArchiveVersion version;
	u32 fileSize;
	u16 sectionCount;
} PLSR_ArchiveHeaderInfo;

/// Archive section header information
typedef struct {
	u32 blockSize; ///< Section size
} PLSR_ArchiveSectionInfo;

/// Archive section
typedef struct {
	u32 offset; ///< Section start offset in archive
	PLSR_ArchiveSectionInfo info; ///< Cached section information
} PLSR_ArchiveSection;

/// Archive section header information
typedef struct {
	u32 count; ///< Table entry/block count
} PLSR_ArchiveTableInfo;

/// Archive table (entry list)
typedef struct {
	u32 offset; ///< Table start offset in archive
	PLSR_ArchiveTableInfo info; ///< Cached table information
} PLSR_ArchiveTable;

/// Archive table entry (fixed size entry)
typedef struct {
	u32 offset; ///< Entry start offset in archive
} PLSR_ArchiveTableEntry;

/// Archive table block (sized entry)
typedef struct {
	u32 offset; ///< Entry start offset in archive
	u32 size;
} PLSR_ArchiveTableBlock;

/// Archive list information
typedef struct {
	u32 entrySize;
	u32 count;
} PLSR_ArchiveListInfo;

/// Archive list (fixed size data array)
typedef struct {
	u32 offset; ///< List start offset in archive
	PLSR_ArchiveListInfo info; ///< Cached list information
} PLSR_ArchiveList;

/// Archive file
/** Created by opening a file, or an embedded archive at a specified offset */
typedef struct {
	PLSR_ArchiveFileHandle handle; ///< File handle, closed when all archives with the same handle are closed
	u32 offset; ///< Start offset of the archive in the file
} PLSR_Archive;

/// Open from a file at specified path
PLSR_RC plsrArchiveOpen(const char* path, PLSR_Archive* out);

/// Open from inside another archive at specified offset
PLSR_RC plsrArchiveOpenInside(const PLSR_Archive* ar, u32 offset, PLSR_Archive* out);

/// Close archive, releasing resources if applicable
void plsrArchiveClose(PLSR_Archive* ar);

/// Read archive contents at the current position
NX_INLINE PLSR_RC plsrArchiveRead(const PLSR_Archive* ar, void* out, size_t size) {
	return plsrArchiveFileRead(ar->handle, out, size) ? PLSR_RC_OK : PLSR_ResultType_NotFound;
}

/// Read archive contents at the specified offset (advanced)
PLSR_RC plsrArchiveReadAtEx(const PLSR_Archive* ar, u32 offset, void* out, size_t size, bool acceptZero);

/// Read archive contents at the specified offset
/** @note Offset zero is invalid unless you specify otherwise using plsrArchiveReadAtEx() (start of an archive should always be the header) */
NX_INLINE PLSR_RC plsrArchiveReadAt(const PLSR_Archive* ar, u32 offset, void* out, size_t size) {
	return plsrArchiveReadAtEx(ar, offset, out, size, false);
}

/// Read a null-terminated string at the specified offset
PLSR_RC plsrArchiveReadString(const PLSR_Archive* ar, u32 offset, char* out, size_t size);

/// Read archive header
/** @param magic Expected header magic */
PLSR_RC plsrArchiveReadHeaderInfo(const PLSR_Archive* ar, const char* magic, PLSR_ArchiveHeaderInfo* out);

/// Read archive section header
/** @param magic Expected section magic */
PLSR_RC plsrArchiveReadSectionHeaderInfo(const PLSR_Archive* ar, u32 offset, const char* magic, PLSR_ArchiveSectionInfo* out);

/// Read archive table header information
PLSR_RC plsrArchiveReadTableHeaderInfo(const PLSR_Archive* ar, u32 offset, PLSR_ArchiveTableInfo* out);

/// Read one archive table entry at specified index
/** @param id Expected reference identifier */
PLSR_RC plsrArchiveReadTableEntry(const PLSR_Archive* ar, const PLSR_ArchiveTable* table, u16 id, u32 index, PLSR_ArchiveTableEntry* out);

/// Read one table block at specified index
/** @param id Expected reference identifier */
PLSR_RC plsrArchiveReadTableBlock(const PLSR_Archive* ar, const PLSR_ArchiveTable* table, u16 id, u32 index, PLSR_ArchiveTableBlock* out);

/// Read a number of list entries starting from the specified index
/** @note Additionally to checking RC, outReadCount might not match the requested count */
PLSR_RC plsrArchiveListReadEntries(const PLSR_Archive* ar, const PLSR_ArchiveList* list, u32 startIndex, u32 count, void* outEntries, u32* outReadCount);

/// Read one list entry
PLSR_RC plsrArchiveListGetEntry(const PLSR_Archive* ar, const PLSR_ArchiveList* list, u32 index, void* out);
