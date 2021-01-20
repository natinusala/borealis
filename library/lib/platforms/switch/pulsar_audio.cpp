/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala
    Copyright (C) 2020-2021  p-sam

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "pulsar_audio.hpp"

#include <borealis/core/logger.hpp>

namespace brls
{

#define QLAUNCH_PID 0x0100000000001000
#define QLAUNCH_MOUNT_POINT "qlaunch"
#define QLAUNCH_BFSAR_PATH "qlaunch:/sound/qlaunch.bfsar"

const std::string SOUNDS_MAP[_SOUND_MAX] = {
    "", // SOUND_NONE
    "SeBtnFocus", // SOUND_FOCUS_CHANGE
    "SeKeyErrorCursor", // SOUND_FOCUS_ERROR
    "SeBtnDecide", // SOUND_CLICK
    "SeNaviFocus", // SOUND_FOCUS_SIDEBAR
    "SeKeyError", // SOUND_CLICK_ERROR
    "SeUnlockKeyZR", // SOUND_HONK
    "SeNaviDecide", // SOUND_CLICK_SIDEBAR
};

PulsarAudioPlayer::PulsarAudioPlayer()
{
    // Init the sounds array
    for (size_t sound = 0; sound < _SOUND_MAX; sound++)
        this->sounds[sound] = PLSR_PLAYER_INVALID_SOUND;

    // Init pulsar player
    // TODO: give a custom config with more than 24 voices if needed
    PLSR_RC rc = plsrPlayerInit();
    if(PLSR_RC_FAILED(rc))
    {
        Logger::error("Unable to init Pulsar player: {#x}", rc);
        return;
    }

    // Mount qlaunch ROMFS for the BFSAR
    Result result = romfsMountDataStorageFromProgram(QLAUNCH_PID, QLAUNCH_MOUNT_POINT);
    if (!R_SUCCEEDED(result))
    {
        Logger::error("Unable to mount qlaunch ROMFS: {#x}", result);

        plsrPlayerExit();
        return;
    }

    // Open qlaunch BFSAR
    rc = plsrBFSAROpen(QLAUNCH_BFSAR_PATH, &this->qlaunchBfsar);
    if(PLSR_RC_FAILED(rc))
    {
        Logger::error("Unable to open qlaunch BFSAR: {#x}", rc);

        plsrPlayerExit();
        return;
    }

    // Good to go~
    this->init = true;
}

bool PulsarAudioPlayer::load(enum Sound sound)
{
    if (!this->init)
        return false;

    if (sound == SOUND_NONE)
        return true;

    if (this->sounds[sound] != PLSR_PLAYER_INVALID_SOUND)
        return true;

    std::string soundName = SOUNDS_MAP[sound];

    if (soundName == "")
        return false; // unimplemented sound

    Logger::debug("Loading sound {}: {}", sound, soundName);

    PLSR_RC rc = plsrPlayerLoadSoundByName(&this->qlaunchBfsar, soundName.c_str(), &this->sounds[sound]);
    if(PLSR_RC_FAILED(rc))
    {
        Logger::warning("Unable to load sound {}: {#x}", soundName, rc);
        this->sounds[sound] = PLSR_PLAYER_INVALID_SOUND;
        return false;
    }

    return true;
}

bool PulsarAudioPlayer::play(enum Sound sound)
{
    if (!this->init)
        return false;

    if (sound == SOUND_NONE)
        return true;

    // Load the sound if needed
    if (this->sounds[sound] == PLSR_PLAYER_INVALID_SOUND)
    {
        if (!this->load(sound))
            return false;
    }

    Logger::debug("Playing sound {}", sound);

    // Play the sound
    PLSR_RC rc = plsrPlayerPlay(this->sounds[sound]);
    if (PLSR_RC_FAILED(rc))
    {
        Logger::error("Unable to play sound {}: {#x}", sound, rc);
        return false;
    }

    return true;
}

PulsarAudioPlayer::~PulsarAudioPlayer()
{
    if (!this->init)
        return;

    // Unload all sounds
    for (size_t sound = 0; sound < _SOUND_MAX; sound++)
    {
        if (this->sounds[sound] != PLSR_PLAYER_INVALID_SOUND)
            plsrPlayerFree(this->sounds[sound]);
    }

    // Close the archive and exit player
    plsrBFSARClose(&this->qlaunchBfsar);
    plsrPlayerExit();
}

} // namespace brls


