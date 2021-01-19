/**
 * @file
 * @brief Player sound loading
 */
#pragma once

#include <pulsar/player/player.h>

/// Load a sound from a Wave file
PLSR_RC plsrPlayerLoadWave(const PLSR_BFWAV* bfwav, PLSR_PlayerSoundId* out);
