#include <pulsar/bfwsd/bfwsd_wave_id.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFWSD, WaveId, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFWSD, WaveId, X)

_Static_assert(sizeof(_PLSR_BFWSDWaveId) == sizeof(PLSR_BFWSDWaveId), "Structs should be the same to permit casting from raw reads");

PLSR_RC plsrBFWSDWaveIdListReadEntries(const PLSR_BFWSD* bfwsd, u32 startIndex, u32 count, PLSR_BFWSDWaveId* outIds, u32* outReadCount) {
	_LOCAL_TRY(plsrArchiveListReadEntries(&bfwsd->ar, &bfwsd->waveIdList, startIndex, count, outIds, outReadCount));
	return PLSR_RC_OK;
}

PLSR_RC plsrBFWSDWaveIdListGetEntry(const PLSR_BFWSD* bfwsd, u32 index, PLSR_BFWSDWaveId* out) {
	_LOCAL_TRY(plsrArchiveListGetEntry(&bfwsd->ar, &bfwsd->waveIdList, index, out));
	return PLSR_RC_OK;
}
