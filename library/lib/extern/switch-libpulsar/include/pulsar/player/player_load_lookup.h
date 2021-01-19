/**
 * @file
 * @brief Player sound archive lookup and loading
 */
#pragma once

#include <pulsar/player/player.h>
#include <pulsar/bfsar/bfsar.h>
#include <pulsar/bfsar/bfsar_item.h>

/// Search sound archive string table for a sound with the specified name and load it
PLSR_RC plsrPlayerLoadSoundByName(const PLSR_BFSAR* bfsar, const char* name, PLSR_PlayerSoundId* out);

/// Find and load a sound from a sound archive with the specified item ID
PLSR_RC plsrPlayerLoadSoundByItemId(const PLSR_BFSAR* bfsar, PLSR_BFSARItemId itemId, PLSR_PlayerSoundId* out);
