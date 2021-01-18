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

namespace brls
{

enum Sound
{
    FOCUS_CHANGE = 0, // played when the focus changes
    FOCUS_ERROR, // played when the user wants to go somewhere impossible (while the highlight wiggles)
    CLICK, // played when the click action runs
};

// Platform agnostic Audio player
// Each platform's AudioPlayer is responsible for managing the enum Sound -> internal representation map
class AudioPlayer
{
  public:
    virtual ~AudioPlayer() {};

    /**
     * Preemptively loads the given sound so that it's ready to be played
     * when needed.
     *
     * Returns a boolean indicating if the sound has been loaded or not.
     */
    virtual bool load(enum Sound sound) = 0;

    /**
     * Plays the given sound.
     *
     * The AudioPlayer should not assume that the sound has been
     * loaded already, and must load it if needed.
     *
     * Returns a boolean indicating if the sound has been played or not.
     */
    virtual bool play(enum Sound sound) = 0;
};

// An AudioPlayer that does nothing
class NullAudioPlayer : public AudioPlayer
{
  public:
    bool load(enum Sound sound) override
    {
        return false;
    }

    bool play(enum Sound sound) override
    {
        return false;
    }
};

} // namespace brls
