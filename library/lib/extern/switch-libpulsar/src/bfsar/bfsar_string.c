#include <pulsar/bfsar/bfsar_string.h>

#define _LOCAL_TRY(X) PLSR_RC_LTRY(BFSAR, String, X)
#define _LOCAL_RC_MAKE(X) PLSR_RC_MAKE(BFSAR, String, X)

PLSR_RC plsrBFSARStringGet(const PLSR_BFSAR* bfsar, u32 index, char* out, size_t size) {
	PLSR_ArchiveTableBlock block;

	_LOCAL_TRY(plsrArchiveReadTableBlock(&bfsar->ar, &bfsar->stringTable, _PLSR_BFSAR_STRG_IDENTIFIER_STRING_TABLE_ENTRY, index, &block));

	size_t stringOutSize = size < block.size ? size : block.size;
	_LOCAL_TRY(plsrArchiveReadAt(&bfsar->ar, block.offset, out, stringOutSize));

	out[size-1] = '\0';
	return PLSR_RC_OK;
}

PLSR_RC plsrBFSARStringSearchEx(const PLSR_BFSAR* bfsar, const char* query, size_t queryLen, PLSR_BFSARStringSearchInfo* out) {
	_PLSR_BFSARStringTreeNode _node;
	char c;
	u32 index = bfsar->stringTree.info.rootNodeIndex;

	while(index < bfsar->stringTree.info.nodeCount) {
		_LOCAL_TRY(plsrArchiveReadAt(&bfsar->ar, bfsar->stringTree.offset + sizeof(_PLSR_BFSARStringTreeHeader) + index * sizeof(_PLSR_BFSARStringTreeNode), &_node, sizeof(_node)));

		if(_node.endpointFlag) {
			break;
		}

		c = (_node.stringBit / 8) >= queryLen ? 0 : query[_node.stringBit / 8];
		index = (c >> (7 - _node.stringBit % 8)) & 1 ? _node.rightIndex : _node.leftIndex;
	}

	PLSR_RC_TRY(plsrBFSARStringGet(bfsar, _node.stringTableIndex, &out->name[0], sizeof(out->name)));

	if(strncmp(out->name, query, sizeof(out->name))) {
		return _LOCAL_RC_MAKE(NotFound);
	}

	out->itemId.raw = _node.itemId;

	return PLSR_RC_OK;
}
