/**
 * @file
 * @brief Base types
 */
#pragma once

#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

/// Shortcut to get a result type enum from its name
#define PLSR_RC_MAKE_RESULT_TYPE(RESULT_TYPE) (PLSR_ResultType_ ## RESULT_TYPE)
/// Shortcut to get a category enum from its name
#define PLSR_RC_MAKE_CATEGORY_TYPE(ARCHIVE_TYPE, CATEGORY_TYPE) (PLSR_ ## ARCHIVE_TYPE ## CategoryType_ ## CATEGORY_TYPE)
/// Shortcut to get an archive enum from its name
#define PLSR_RC_MAKE_ARCHIVE_TYPE(ARCHIVE_TYPE) (PLSR_ArchiveType_ ## ARCHIVE_TYPE)

/// Make a result code from raw integers
#define PLSR_RC_MAKE_RAW(ARCHIVE, CATEGORY, RESULT) (RESULT + (CATEGORY << 8) + (ARCHIVE << 16))

/// Make a result code from archive, category and result type names
#define PLSR_RC_MAKE(ARCHIVE_TYPE, CATEGORY_TYPE, RESULT_TYPE) PLSR_RC_MAKE_RAW( \
	PLSR_RC_MAKE_ARCHIVE_TYPE(ARCHIVE_TYPE), \
	PLSR_RC_MAKE_CATEGORY_TYPE(ARCHIVE_TYPE, CATEGORY_TYPE), \
	PLSR_RC_MAKE_RESULT_TYPE(RESULT_TYPE) \
)

/// Result code returned on success
#define PLSR_RC_OK 0
/// Test that the result code means failure
#define PLSR_RC_FAILED(RESULT) (RESULT != PLSR_RC_OK)
/// Test that the result code means success
#define PLSR_RC_SUCCEEDED(RESULT) (RESULT == PLSR_RC_OK)

/// Extract the archive part of a result code
#define PLSR_RC_ARCHIVE(RESULT) ((RESULT >> 16) & 0xFF)
/// Extract the category part of a result code
#define PLSR_RC_CATEGORY(RESULT) ((RESULT >> 8) & 0xFF)
/// Extract the result type part of a result code
#define PLSR_RC_RESULT(RESULT) (RESULT & 0xFF)

/// Convert a result code to another archive and category, keeping the result type
/** @note This is mainly meant to convert errors from generic archive read calls into the callee archive and category type */
#define PLSR_RC_CONVERT(RESULT, NEW_ARCHIVE_TYPE, NEW_CATEGORY_TYPE) PLSR_RC_MAKE_RAW( \
	PLSR_RC_MAKE_ARCHIVE_TYPE(NEW_ARCHIVE_TYPE), \
	PLSR_RC_MAKE_CATEGORY_TYPE(NEW_ARCHIVE_TYPE, NEW_CATEGORY_TYPE), \
	PLSR_RC_RESULT(RESULT) \
)

/// Return result code if failed
#define PLSR_RC_TRY(X) do { \
	const PLSR_RC _rc = (X); \
	if(PLSR_RC_FAILED(_rc)) { return _rc; } \
} while(false)

/// Return result code if failed, also converts result to a new archive and category
/** @note This is mainly meant to convert errors from generic archive read calls into the callee archive and category type */
#define PLSR_RC_LTRY(ARCHIVE_TYPE, CATEGORY_TYPE, X) do { \
	const PLSR_RC _rc = (X); \
	if(PLSR_RC_FAILED(_rc)) { return PLSR_RC_CONVERT(_rc, ARCHIVE_TYPE, CATEGORY_TYPE); } \
} while(false)

/// Return a System type result code if the libnx result failed
/** @note This is mainly used in the player when calling the audio renderer */
#define PLSR_RC_NX_LTRY(ARCHIVE_TYPE, CATEGORY_TYPE, NX_RESULT) do { \
	if(R_FAILED(NX_RESULT)) { return PLSR_RC_MAKE(ARCHIVE_TYPE, CATEGORY_TYPE, System); } \
} while(false)

/// Result code returned by Pulsar functions
typedef u32 PLSR_RC;

/// Result code types
typedef enum {
	PLSR_ResultType_OK = 0,

	PLSR_ResultType_FileRead, ///< File could not be opened or read
	PLSR_ResultType_BadMagic, ///< Magic mismatch
	PLSR_ResultType_BadEndianness, ///< Archive endianness did not match host endianness
	PLSR_ResultType_BadInput, ///< Function was called with improper arguments
	PLSR_ResultType_NotFound, ///< Requested data could not be retrieved
	PLSR_ResultType_NotReady, ///< Function was called before required initialization
	PLSR_ResultType_Unsupported, ///< Execution encountered an unexpected case
	PLSR_ResultType_Memory, ///< Memory allocation failed

	PLSR_ResultType_System = 0xFF ///< Can indicate an underlying libnx fn call failure
} PLSR_ResultType;

/// Supported archive types
typedef enum {
	PLSR_ArchiveType_Unknown = 0,

	PLSR_ArchiveType_BFSAR, ///< Sound archive
	PLSR_ArchiveType_BFGRP, ///< Sound group
	PLSR_ArchiveType_BFWSD, ///< Wave sound data
	PLSR_ArchiveType_BFWAR, ///< Wave archive
	PLSR_ArchiveType_BFWAV, ///< Wave file

	PLSR_ArchiveType_Player = 0xFF, ///< Not an archive, type used by Player functions
} PLSR_ArchiveType;
