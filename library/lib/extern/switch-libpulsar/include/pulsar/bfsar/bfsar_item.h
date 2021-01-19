/**
 * @file
 * @brief Sound archive item id types
 */
#pragma once

#include <pulsar/bfsar/bfsar.h>

/// Item types
typedef enum {
	PLSR_BFSARItemType_Unknown = 0,

	/// (Sound) use plsrBFSARSoundGet() with index
	PLSR_BFSARItemType_Sound = _PLSR_BFSAR_FILE_TYPE_SOUND,

	/// (Sound Set) use plsrBFSARSoundSetGet() with index
	PLSR_BFSARItemType_SoundSet = _PLSR_BFSAR_FILE_TYPE_SOUND_SET,

	/// (Bank) Unimplemented
	PLSR_BFSARItemType_Bank = _PLSR_BFSAR_FILE_TYPE_BANK,

	/// (Player settings) Unimplemented
	PLSR_BFSARItemType_Player = _PLSR_BFSAR_FILE_TYPE_PLAYER,

	/// (Wave Archive) use plsrBFSARWaveArchiveGet() with index
	PLSR_BFSARItemType_WaveArchive = _PLSR_BFSAR_FILE_TYPE_WAVE_ARCHIVE,

	/// (Group) use plsrBFSARGroupGet() with index
	PLSR_BFSARItemType_Group = _PLSR_BFSAR_FILE_TYPE_GROUP,
} PLSR_BFSARItemType;

/// Id for one item/resource contained in the sound archive
/** Item ids contain the item type to decide which table to look in, and the index of the item in that table */
typedef union {
	uint32_t raw;
	struct {
		uint32_t index : _PLSR_BFSAR_ITEM_ID_INDEX_BITS;
		PLSR_BFSARItemType type : _PLSR_BFSAR_ITEM_ID_TYPE_BITS;
	};
} PLSR_BFSARItemId;
