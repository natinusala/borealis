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

namespace brls
{

class SwitchPlatform : public GLFWPlatform // TODO: don't inherit from GLFWPlatform anymore
{
  public:
    SwitchPlatform(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight);
    ~SwitchPlatform();

    std::string getName() override;

    bool mainLoopIteration() override;

    AudioPlayer* getAudioPlayer() override;

  private:
    SwitchAudioPlayer* audioPlayer;
};

} // namespace brls
