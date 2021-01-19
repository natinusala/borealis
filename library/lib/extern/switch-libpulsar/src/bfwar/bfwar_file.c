#include <pulsar/bfwar/bfwar_file.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFWAR, File, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFWAR, File, X)

PLSR_RC plsrBFWARFileGet(const PLSR_BFWAR* bfwar, u32 index, PLSR_BFWARFileInfo* out) {
	PLSR_ArchiveTableBlock tableBlock;
	_LOCAL_TRY(plsrArchiveReadTableBlock(&bfwar->ar, &bfwar->fileTable, _PLSR_BFWAR_FILE_IDENTIFIER_BLOCK_ENTRY, index, &tableBlock));

	out->offset = tableBlock.offset + bfwar->fileSection.offset + sizeof(_PLSR_ArchiveSectionHeader) - bfwar->fileTable.offset;
	out->size = tableBlock.size;
	return PLSR_RC_OK;
}
