/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala

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

#pragma once

#include <borealis/core/audio.hpp>

#include <pulsar.h>

namespace brls
{

// AudioPlayer implementation that uses libpulsar to load and play
// qlaunch sounds loaded at runtime
class PulsarAudioPlayer : public AudioPlayer
{
  public:
    PulsarAudioPlayer();
    ~PulsarAudioPlayer();

    bool load(enum Sound sound) override;
    bool play(enum Sound sound) override;

  private:
    bool init = false;

    PLSR_BFSAR qlaunchBfsar;

    PLSR_PlayerSoundId sounds[_BRLS_SOUND_MAX];
};

} //namespace brls
