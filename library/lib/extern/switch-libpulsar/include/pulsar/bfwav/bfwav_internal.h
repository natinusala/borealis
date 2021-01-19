#pragma once

// @see https://github.com/Kinnay/Nintendo-File-Formats/wiki/BFWAR-File-Format

#include <pulsar/types.h>
#include <pulsar/archive/archive_internal.h>

#define _PLSR_BFWAV_MAGIC "FWAV"
#define _PLSR_BFWAV_INFO_MAGIC "INFO"
#define _PLSR_BFWAV_DATA_MAGIC "DATA"

#define _PLSR_BFWAV_SECTION_IDENTIFIER_INFO 0x7000
#define _PLSR_BFWAV_SECTION_IDENTIFIER_DATA 0x7001

#define _PLSR_BFWAV_INFO_IDENTIFIER_CHANNEL_ENTRY 0x7100

#define _PLSR_BFWAV_INFO_IDENTIFIER_DATA_ENTRY 0x1f00
#define _PLSR_BFWAV_INFO_IDENTIFIER_ADPCM_ENTRY 0x0300

#define _PLSR_BFWAV_FORMAT_PCM_8 0
#define _PLSR_BFWAV_FORMAT_PCM_16 1
#define _PLSR_BFWAV_FORMAT_DSP_ADPCM 2

typedef struct {
	u8 format;
	u8 looping;
	char _padX03[2];
	u32 sampleRate;
	u32 loopStartSample;
	u32 sampleCount;
	char _padX11[4];
} _PLSR_BFWAVInfo;

typedef struct {
	_PLSR_ArchiveRef dataRef;
	_PLSR_ArchiveRef adpcmInfoRef;
} _PLSR_BFWAVChannelInfo;
