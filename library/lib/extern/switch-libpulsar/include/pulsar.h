/**
 * @file
 * @brief Main header meant to be included in user projects
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <pulsar/types.h>
#include <pulsar/archive/archive.h>

#include <pulsar/bfgrp/bfgrp.h>
#include <pulsar/bfgrp/bfgrp_location.h>

#include <pulsar/bfsar/bfsar.h>
#include <pulsar/bfsar/bfsar_item.h>
#include <pulsar/bfsar/bfsar_string.h>
#include <pulsar/bfsar/bfsar_file.h>
#include <pulsar/bfsar/bfsar_group.h>
#include <pulsar/bfsar/bfsar_sound.h>
#include <pulsar/bfsar/bfsar_wave_archive.h>

#include <pulsar/bfwar/bfwar.h>
#include <pulsar/bfwar/bfwar_file.h>

#include <pulsar/bfwav/bfwav.h>
#include <pulsar/bfwav/bfwav_info.h>

#include <pulsar/bfwsd/bfwsd.h>
#include <pulsar/bfwsd/bfwsd_wave_id.h>
#include <pulsar/bfwsd/bfwsd_sound_data.h>

#ifdef __SWITCH__

#include <pulsar/player/player.h>
#include <pulsar/player/player_load.h>
#include <pulsar/player/player_load_lookup.h>

#endif

#ifdef __cplusplus
}
#endif