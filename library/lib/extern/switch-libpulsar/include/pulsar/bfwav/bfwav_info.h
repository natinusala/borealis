/**
 * @file
 * @brief Wave file info
 */
#pragma once

#include <pulsar/bfwav/bfwav.h>

/// Wave channel ADPCM context information
typedef struct {
	u16 header; ///< Initial header (predictor/scale)
	s16 yn1; ///< Sample history 1
	s16 yn2; ///< Sample history 2
} PLSR_BFWAVAdpcmContextInfo;

/// Wave channel ADPCM information (coefficients and contexts)
typedef struct {
	u16 coeffs[16]; ///< ADPCM coefficients
	PLSR_BFWAVAdpcmContextInfo main; ///< ADPCM main context
	PLSR_BFWAVAdpcmContextInfo loop; ///< ADPCM loop context (if wave info `looping` flag is set)
} PLSR_BFWAVAdpcmInfo;

/// Wave channel information
typedef struct {
	u32 dataOffset; ///< Offset to channel sample data relative to the start of the file (see plsrArchiveReadAt() to read)
	PLSR_BFWAVAdpcmInfo adpcmInfo; ///< ADPCM information (if wave sample format is PLSR_BFWAVFormat_DSP_ADPCM)
} PLSR_BFWAVChannelInfo;

/// Wave channel information table
typedef PLSR_ArchiveTable PLSR_BFWAVChannelInfoTable;

/// Wave information
typedef struct {
	PLSR_BFWAVFormat format; ///< Sample format
	bool looping; ///< `true` if the file contains a looping section
	u32 sampleRate; ///< Sample rate
	u32 loopStartSample; ///< Sample index where the loop starts (if looping is `true`)
	u32 sampleCount; ///< Sample count
	PLSR_BFWAVChannelInfoTable channelInfoTable; ///< Channel Information table
} PLSR_BFWAVInfo;

/// Read wave file information from header
PLSR_RC plsrBFWAVReadInfo(const PLSR_BFWAV* bfwav, PLSR_BFWAVInfo* out);

/// Read wave channel information from the specified index in the channel info table
PLSR_RC plsrBFWAVReadChannelInfo(const PLSR_BFWAV* bfwav, const PLSR_BFWAVChannelInfoTable* table, u32 index, PLSR_BFWAVChannelInfo* out);
