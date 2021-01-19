/**
 * @file
 * @brief Wave sound data access
 */
#pragma once

#include <pulsar/bfwsd/bfwsd.h>

/// Wave sound data note information table
typedef PLSR_ArchiveTable PLSR_BFWSDNoteInfoTable;

/// Wave sound data ADSHR curve
typedef struct {
	u8 attack;
	u8 decay;
	u8 sustain;
	u8 hold;
	u8 release;
} PLSR_BFWSDAdshrCurve;

/// Wave sound data send values
typedef struct {
	u8 main;
	u8 auxCount;
	u8 aux[3];
} PLSR_BFWSDSendValues;

/// Wave sound data note information
typedef struct {
	u32 waveIdIndex; ///< Related index in the wave id table
} PLSR_BFWSDNoteInfo;

/// Wave sound data information
typedef struct {
	PLSR_BFWSDNoteInfoTable noteInfoTable;

	bool hasPan;
	u8 pan; ///< Populated if `hasPan` is `true`
	u8 surroundPan; ///< Populated if `hasPan` is `true`

	bool hasPitch;
	float pitch; ///< Populated if `hasPitch` is `true`

	bool hasSend;
	PLSR_BFWSDSendValues send; ///< Populated if `hasSend` is `true`

	bool hasAdshr;
	PLSR_BFWSDAdshrCurve adshr; ///< Populated if `hasAdshr` is `true`
} PLSR_BFWSDSoundDataInfo;

/// Fetch wave sound data information from the specified index in the table
PLSR_RC plsrBFWSDSoundDataGet(const PLSR_BFWSD* bfwsd, u32 index, PLSR_BFWSDSoundDataInfo* out);

/// Fetch note information from the specified index in the wave sound data note table
PLSR_RC plsrBFWSDSoundDataNoteGet(const PLSR_BFWSD* bfwsd, const PLSR_BFWSDNoteInfoTable* table, u32 index, PLSR_BFWSDNoteInfo* out);

NX_INLINE u32 plsrBFWSDSoundDataCount(const PLSR_BFWSD* bfwsd) {
	return bfwsd->soundDataTable.info.count;
}
