#pragma once

#include <pulsar/types.h>
#include <pulsar/archive/archive_internal.h>

// @see https://github.com/Kinnay/Nintendo-File-Formats/wiki/BFWSD-File-Format
// @see https://github.com/Gota7/Citric-Composer/blob/master/Citric%20Composer/Citric%20Composer/High%20Level/Wave%20Sound%20Data/WaveSoundData.cs

#define _PLSR_BFWSD_MAGIC "FWSD"
#define _PLSR_BFWSD_INFO_MAGIC "INFO"

#define _PLSR_BFWSD_SECTION_IDENTIFIER_INFO 0x6800

#define _PLSR_BFWSD_INFO_IDENTIFIER_WAVE_ID_TABLE 0x0100
#define _PLSR_BFWSD_INFO_IDENTIFIER_SOUND_DATA_TABLE 0x0101
#define _PLSR_BFWSD_INFO_IDENTIFIER_NOTE_TABLE 0x0101

#define _PLSR_BFWSD_INFO_IDENTIFIER_SOUND_DATA_ENTRY 0x4900
#define _PLSR_BFWSD_INFO_IDENTIFIER_SOUND_INFO_ENTRY 0x4901
#define _PLSR_BFWSD_INFO_IDENTIFIER_NOTE_ENTRY 0x4902
#define _PLSR_BFWSD_INFO_IDENTIFIER_ADSHR_ENTRY 0x0000

#define _PLSR_BFWSD_FLAG_SOUND_INFO_PAN BIT(0)
#define _PLSR_BFWSD_FLAG_SOUND_INFO_PITCH BIT(1)
#define _PLSR_BFWSD_FLAG_SOUND_INFO_SEND BIT(8)
#define _PLSR_BFWSD_FLAG_SOUND_INFO_ADSHR BIT(9)

typedef struct {
	u32 archiveItemId;
	u32 index;
} _PLSR_BFWSDWaveId;

typedef struct {
	_PLSR_ArchiveRef soundInfoRef;
	_PLSR_ArchiveRef trackInfoRef;
	_PLSR_ArchiveRef noteInfoTableRef;
} _PLSR_BFWSDSoundDataEntry;

typedef struct {
	u32 flags;
} _PLSR_BFWSDSoundInfoEntry;

typedef struct {
	u32 waveIdIndex;
	u32 flags;
} _PLSR_BFWSDNoteEntry;
