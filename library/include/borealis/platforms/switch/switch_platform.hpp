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

#include <borealis/platforms/glfw/glfw_platform.hpp>
#include <borealis/platforms/switch/switch_audio.hpp>
#include <borealis/platforms/switch/switch_font.hpp>
#include <borealis/platforms/switch/switch_input.hpp>
#include <borealis/platforms/switch/switch_video.hpp>

namespace brls
{

class SwitchPlatform : public Platform
{
  public:
    SwitchPlatform();
    ~SwitchPlatform();

    void createWindow(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight) override;

    std::string getName() override;

    bool mainLoopIteration() override;
    ThemeVariant getThemeVariant() override;
    std::string getLocale() override;

    VideoContext* getVideoContext() override;
    AudioPlayer* getAudioPlayer() override;
    InputManager* getInputManager() override;
    FontLoader* getFontLoader() override;

    void appletCallback(AppletHookType hookType);

  private:
    AppletHookCookie appletCookie;

    ThemeVariant themeVariant;
    std::string locale;

    SwitchAudioPlayer* audioPlayer;
    SwitchInputManager* inputManager;
    SwitchVideoContext* videoContext;
    SwitchFontLoader* fontLoader;
};

} // namespace brls
