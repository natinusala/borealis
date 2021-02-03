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

#include <borealis/platforms/glfw/glfw_platform.hpp>
#include <borealis/platforms/switch/switch_audio.hpp>
#include <borealis/platforms/switch/switch_input.hpp>
#include <borealis/platforms/switch/switch_video.hpp>
#include <borealis/platforms/switch/switch_touch.hpp>

namespace brls
{

class SwitchPlatform : public Platform
{
  public:
    SwitchPlatform();
    ~SwitchPlatform();

    void init() override;

    std::string getName() override;

    bool mainLoopIteration() override;
    ThemeVariant getThemeVariant() override;

    VideoContext* getVideoContext() override;
    AudioPlayer* getAudioPlayer() override;
    InputManager* getInputManager() override;
    TouchManager* getTouchManager() override;

    void appletCallback(AppletHookType hookType);

  private:
    AppletHookCookie appletCookie;

    ThemeVariant themeVariant;

    SwitchAudioPlayer* audioPlayer;
    SwitchInputManager* inputManager;
    SwitchVideoContext* videoContext;
    SwitchTouchManager* touchManager;
};

} // namespace brls
