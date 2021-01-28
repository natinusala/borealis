/*
** Sample Framework for deko3d Applications
**   FileLoader.h: Helpers for loading data from the filesystem directly into GPU memory
*/
#pragma once
#include "CMemPool.h"
#include "common.h"

CMemPool::Handle LoadFile(CMemPool& pool, const char* path, uint32_t alignment = DK_CMDMEM_ALIGNMENT);
