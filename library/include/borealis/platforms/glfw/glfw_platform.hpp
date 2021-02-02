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

#include <borealis/core/platform.hpp>
#include <borealis/platforms/glfw/glfw_input.hpp>
#include <borealis/platforms/glfw/glfw_video.hpp>

namespace brls
{

class GLFWPlatform : public Platform
{
  public:
    GLFWPlatform(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight);
    ~GLFWPlatform();

    std::string getName() override;
    void init() override;

    bool mainLoopIteration() override;
    ThemeVariant getThemeVariant() override;

    AudioPlayer* getAudioPlayer() override;
    VideoContext* getVideoContext() override;
    InputManager* getInputManager() override;

  private:
    NullAudioPlayer* audioPlayer   = nullptr;
    GLFWVideoContext* videoContext = nullptr;
    GLFWInputManager* inputManager = nullptr;
};

} // namespace brls
