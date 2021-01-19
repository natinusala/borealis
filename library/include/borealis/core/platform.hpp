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
#include <string>

namespace brls
{

// Interface to provide everything platform specific required to run borealis: graphics context, inputs, audio...
// The best platform is automatically selected when the application starts, and cannot be changed by the user at the moment
class Platform
{
  public:
    virtual ~Platform();

    /**
     * Returns the human-readable name of the platform.
     */
    virtual std::string getName() = 0;

    /**
     * Creates the audio player if needed and returns it.
     */
    AudioPlayer* getAudioPlayer();

    /**
     * Selects and returns the best platform.
     */
    static Platform* createPlatform();

  protected:
    /**
     * Returns an AudioPlayer instance for the platform.
     * The instance is automatically freed by the Application when it exits.
     */
    virtual AudioPlayer* createAudioPlayer();

  private:
    AudioPlayer* audioPlayer = nullptr;
};

} // namespace brls
