/*
    Copyright 2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
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
     * Must return false if the app should continue running
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
