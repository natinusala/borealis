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

#include <borealis/core/platform.hpp>
#include <borealis/platforms/glfw/glfw_font.hpp>
#include <borealis/platforms/glfw/glfw_input.hpp>
#include <borealis/platforms/glfw/glfw_video.hpp>

namespace brls
{

class GLFWPlatform : public Platform
{
  public:
    GLFWPlatform();
    ~GLFWPlatform();

    std::string getName() override;
    void createWindow(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight) override;

    bool mainLoopIteration() override;
    ThemeVariant getThemeVariant() override;
    std::string getLocale() override;

    AudioPlayer* getAudioPlayer() override;
    VideoContext* getVideoContext() override;
    InputManager* getInputManager() override;
    FontLoader* getFontLoader() override;

  private:
    NullAudioPlayer* audioPlayer   = nullptr;
    GLFWVideoContext* videoContext = nullptr;
    GLFWInputManager* inputManager = nullptr;
    GLFWFontLoader* fontLoader     = nullptr;
};

} // namespace brls
