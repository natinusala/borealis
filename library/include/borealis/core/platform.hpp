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
#include <borealis/core/font.hpp>
#include <borealis/core/input.hpp>
#include <borealis/core/theme.hpp>
#include <borealis/core/video.hpp>
#include <string>

namespace brls
{

// Interface to provide everything platform specific required to run borealis: graphics context, inputs, audio...
// The best platform is automatically selected when the application starts, and cannot be changed by the user at the moment
class Platform
{
  public:
    virtual ~Platform() {};

    /**
     * Called on startup, right after instanciation, to create and open a window
     * with the given title and size.
     */
    virtual void createWindow(std::string title, uint32_t width, uint32_t height) = 0;

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
     * Can be called at anytime to get the current system theme variant.
     *
     * For now, the variant is assumed to stay the same during the whole time
     * the app is running (no variant hot swap).
     *
     * As such, the result should be cached by the platform code.
     */
    virtual ThemeVariant getThemeVariant() = 0;

    /**
     * Can be called at anytime to get the current locale
     *
     * For now, the locale is assumed to stay the same during the whole time
     * the app is running (no locale hot swap)
     *
     * As such, the result should be cached by the platform code.
     * The method should return one of the locale constants
     * defined in the i18n header file.
     */
    virtual std::string getLocale() = 0;

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
 	 * Returns the FontLoader for the platform.
     * Cannot return nullptr.
     */
    virtual FontLoader* getFontLoader() = 0;

    /**
     * Selects and returns the best platform.
     */
    static Platform* createPlatform();
};

} // namespace brls
