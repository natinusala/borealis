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
#include <borealis/core/input.hpp>
#include <borealis/core/video.hpp>
#include <string>

namespace brls
{

// Interface to provide everything platform specific required to run borealis: graphics context, inputs, audio...
// The best platform is automatically selected when the application starts, and cannot be changed by the user at the moment
class Platform
{
  public:
    /**
     * Creates the Platform instance with the given window size.
     */
    virtual ~Platform() {};

    /**
     * Returns the human-readable name of the platform.
     */
    virtual std::string getName() = 0;

    /**
     * Called at every iteration of the main loop.
     * Must return if the app should continue running
     * (for example, return false if the X button was pressed on the window).
     */
    virtual bool mainLoopIteration() = 0;

    /**
     * Returns the AudioPlayer for the platform.
     * Cannot return nullptr.
     */
    virtual AudioPlayer* getAudioPlayer() = 0;

    /**
     * Returns the VideoContext for the platform.
     * Cannot return nullptr.
     */
    virtual VideoContext* getVideoContext() = 0;

    /**
     * Returns the InputManager for the platform.
     * Cannot return nullptr.
     */
    virtual InputManager* getInputManager() = 0;

    /**
     * Selects and returns the best platform.
     */
    static Platform* createPlatform(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight);
};

} // namespace brls
