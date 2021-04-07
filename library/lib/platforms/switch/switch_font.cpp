/*
    Copyright 2019 p-sam
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

#include <borealis/core/application.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/core/i18n.hpp>
#include <borealis/platforms/switch/switch_font.hpp>

namespace brls
{

void SwitchFontLoader::loadFonts()
{
    PlFontData font;
    Result rc;

    // Standard
    rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
    if (R_SUCCEEDED(rc))
        Application::loadFontFromMemory(FONT_STANDARD_REGULAR, font.address, font.size, false);
    else
        Logger::error("switch: could not load Standard shared font: {:#x}", rc);

    {
        // Detect if non applet
        bool isFullFallback = false;
        AppletType at = appletGetAppletType();
        if (at == AppletType_Application || at == AppletType_SystemApplication) // title takeover
        {
            isFullFallback = true;
            Logger::info("switch: non applet mode, all shared font will be loaded!");
        } else {
            Logger::info("switch: applet mode, only shared font for current locale will be loaded!");
        }
        std::string locale = Application::getLocale();

        if (locale == LOCALE_ZH_CN || locale == LOCALE_ZH_HANS || isFullFallback)
        {
            // S.Chinese
            rc = plGetSharedFontByType(&font, PlSharedFontType_ChineseSimplified);
            if (R_SUCCEEDED(rc))
                Application::loadFontFromMemory(FONT_SCHINESE_REGULAR, font.address, font.size, false);
            else
                Logger::error("switch: could not load S.Chinese shared font: {:#x}", rc);

            // Ext S.Chinese
            rc = plGetSharedFontByType(&font, PlSharedFontType_ExtChineseSimplified);
            if (R_SUCCEEDED(rc))
                Application::loadFontFromMemory(FONT_SCHINESE_EXTEND, font.address, font.size, false);
            else
                Logger::error("switch: could not load Ext. S.Chinese shared font: {:#x}", rc);
        }

        if (locale == LOCALE_ZH_TW || locale == LOCALE_ZH_HANT || isFullFallback)
        {
            // T.Chinese
            rc = plGetSharedFontByType(&font, PlSharedFontType_ChineseTraditional);
            if (R_SUCCEEDED(rc))
                Application::loadFontFromMemory(FONT_TCHINESE_REGULAR, font.address, font.size, false);
            else
                Logger::error("switch: could not load T.Chinese shared font: {:#x}", rc);
        }

        if (locale == LOCALE_KO || isFullFallback)
        {
            // Korean
            rc = plGetSharedFontByType(&font, PlSharedFontType_KO);
            if (R_SUCCEEDED(rc))
                Application::loadFontFromMemory(FONT_KOREAN_REGULAR, font.address, font.size, false);
            else
                Logger::error("switch: could not load Korean shared font: {:#x}", rc);
        }
    }

    // Extented (symbols)
    rc = plGetSharedFontByType(&font, PlSharedFontType_NintendoExt);
    if (R_SUCCEEDED(rc))
        Application::loadFontFromMemory(FONT_SWITCH_ICONS, font.address, font.size, false);
    else
        Logger::error("switch: could not load Extented shared font: {:#x}", rc);

    // Material icons
    if (!this->loadMaterialFromResources())
        Logger::error("switch: could not load Material icons font from resources", rc);
}

} // namespace brls
