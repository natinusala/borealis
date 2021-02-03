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

#include <switch.h>

#include <borealis/core/logger.hpp>
#include <borealis/platforms/switch/switch_platform.hpp>

namespace brls
{

static void switchAppletCallback(AppletHookType hookType, void* param)
{
    SwitchPlatform* platform = (SwitchPlatform*)param;
    platform->appletCallback(hookType);
}

SwitchPlatform::SwitchPlatform()
{
    appletHook(&this->appletCookie, switchAppletCallback, this);

    // Cache theme variant before video context init
    // The background color is created once in the "static" command list
    // executed every frame, so we need to know the background color
    // to add the clear command to that list.
    ColorSetId colorSetId;
    setsysGetColorSetId(&colorSetId);

    if (colorSetId == ColorSetId_Dark)
        this->themeVariant = ThemeVariant::DARK;
    else
        this->themeVariant = ThemeVariant::LIGHT;

    Logger::info("switch: system has color set {}, using borealis theme {}", colorSetId, this->themeVariant);
}

void SwitchPlatform::init()
{
    this->videoContext = new SwitchVideoContext();
    this->audioPlayer  = new SwitchAudioPlayer();
    this->inputManager = new SwitchInputManager();
    this->touchManager = new SwitchTouchManager();
}

void SwitchPlatform::appletCallback(AppletHookType hookType)
{
    this->videoContext->appletCallback(hookType);
}

std::string SwitchPlatform::getName()
{
    return "Switch";
}

bool SwitchPlatform::mainLoopIteration()
{
    return appletMainLoop();
}

VideoContext* SwitchPlatform::getVideoContext()
{
    return this->videoContext;
}

AudioPlayer* SwitchPlatform::getAudioPlayer()
{
    return this->audioPlayer;
}

InputManager* SwitchPlatform::getInputManager()
{
    return this->inputManager;
}

TouchManager* SwitchPlatform::getTouchManager()
{
    return this->touchManager;
}
  
ThemeVariant SwitchPlatform::getThemeVariant()
{
    return this->themeVariant;
}

SwitchPlatform::~SwitchPlatform()
{
    appletUnhook(&this->appletCookie);

    delete this->audioPlayer;
    delete this->inputManager;
    delete this->videoContext;
}

} // namespace brls
