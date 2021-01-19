/**
 * @file
 * @brief Wave file init
 */
#pragma once

#include <pulsar/archive/archive.h>
#include <pulsar/bfwav/bfwav_internal.h>

/// Wave file method categories
typedef enum {
	PLSR_BFWAVCategoryType_Init = 0,
	PLSR_BFWAVCategoryType_Info,
	PLSR_BFWAVCategoryType_Data,
} PLSR_BFWAVCategoryType;

/// Wave file sample formats
typedef enum {
	/// 8-bit PCM encoded samples
	PLSR_BFWAVFormat_PCM_8 = _PLSR_BFWAV_FORMAT_PCM_8,

	/// 16-bit PCM encoded samples
	PLSR_BFWAVFormat_PCM_16 = _PLSR_BFWAV_FORMAT_PCM_16,

	/// DSP ADPCM encoded samples (Wave channel info should contain the needed adpcm context to decode samples, see plsrBFWAVReadChannelInfo())
	PLSR_BFWAVFormat_DSP_ADPCM = _PLSR_BFWAV_FORMAT_DSP_ADPCM,
} PLSR_BFWAVFormat;

/// Wave file
typedef struct {
	PLSR_Archive ar;

	PLSR_ArchiveHeaderInfo headerInfo;

	PLSR_ArchiveSection infoSection;
	PLSR_ArchiveSection dataSection;
} PLSR_BFWAV;

/// @copydoc plsrArchiveOpen
PLSR_RC plsrBFWAVOpen(const char* path, PLSR_BFWAV* out);

/// @copydoc plsrArchiveOpenInside
PLSR_RC plsrBFWAVOpenInside(const PLSR_Archive* ar, u32 offset, PLSR_BFWAV* out);

/// @copydoc plsrArchiveClose
void plsrBFWAVClose(PLSR_BFWAV* bfwav);
