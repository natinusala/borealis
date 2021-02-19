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

#include <switch.h>

#include <borealis/core/i18n.hpp>
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

    // Init platform impls
    this->audioPlayer  = new SwitchAudioPlayer();
    this->inputManager = new SwitchInputManager();
    this->fontLoader   = new SwitchFontLoader();

    // Get locale
    uint64_t languageCode = 0;

    Result rc = setGetSystemLanguage(&languageCode);

    if (R_SUCCEEDED(rc))
    {
        char* languageName = (char*)&languageCode;
        this->locale       = std::string(languageName);
    }
    else
    {
        Logger::error("switch: unable to get system language (error {:#x}), using the default one: {1}", rc, LOCALE_DEFAULT);
        this->locale = LOCALE_DEFAULT;
    }
}

void SwitchPlatform::createWindow(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
{
    this->videoContext = new SwitchVideoContext();
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

std::string SwitchPlatform::getLocale()
{
    return this->locale;
}

AudioPlayer* SwitchPlatform::getAudioPlayer()
{
    return this->audioPlayer;
}

InputManager* SwitchPlatform::getInputManager()
{
    return this->inputManager;
}

FontLoader* SwitchPlatform::getFontLoader()
{
    return this->fontLoader;
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
