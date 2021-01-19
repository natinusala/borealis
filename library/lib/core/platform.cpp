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

#include <borealis/core/platform.hpp>

#ifdef __SWITCH__
#include <borealis/platforms/switch/switch_platform.hpp>
#endif

#ifdef __GLFW__
#include <borealis/platforms/glfw/glfw_platform.hpp>
#endif

namespace brls
{

Platform* Platform::createPlatform()
{
#if defined(__SWITCH__)
    return new SwitchPlatform();
#elif defined(__GLFW__)
    return new GLFWPlatform();
#endif

    return nullptr;
}

AudioPlayer* Platform::getAudioPlayer()
{
    if (!this->audioPlayer)
        this->audioPlayer = this->createAudioPlayer();

    return this->audioPlayer;
}

AudioPlayer* Platform::createAudioPlayer()
{
    return new NullAudioPlayer();
}

Platform::~Platform()
{
    delete this->audioPlayer;
}

} // namespace brls
