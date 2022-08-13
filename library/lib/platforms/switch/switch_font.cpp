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
    Result rc;
    PlFontData fonts[PlSharedFontType_Total];
    PlFontData* fontsSorted[PlSharedFontType_Total];
    std::string locale = Application::getLocale();

    bool useStandardOnly = false;
    // Compile-time flag
    #ifdef SW_STANDARD_FONT_ONLY
        bool useStandardOnly = true;
        Logger::warning("switch: saving RAM: use standard shared font only!");
    #endif

    // todo: plGetSharedFont must use (u64)languageCode, conflict with suggested use of (std::string)locale
    uint64_t languageCode = 0;
    rc = setGetSystemLanguage(&languageCode);
    if (R_SUCCEEDED(rc))
    {
        int total_fonts=0;
        rc = plGetSharedFont(languageCode, fonts, PlSharedFontType_Total, &total_fonts);
        if (R_SUCCEEDED(rc))
        {
            for (int i=0; i<PlSharedFontType_Total; i++) {
                fontsSorted[fonts[i].type] = &fonts[i];
            }

            // Standard
            Application::loadFontFromMemory(FONT_HOS_STANDARD,
                fontsSorted[PlSharedFontType_Standard]->address,
                fontsSorted[PlSharedFontType_Standard]->size, false);

            if (!useStandardOnly)
            {
                bool isFullFallback = false;
                // Detect if non applet
                AppletType at = appletGetAppletType();
                if (at == AppletType_Application || at == AppletType_SystemApplication) // title takeover
                    isFullFallback = true;
                
                // Compile-time flag
                #ifdef SW_DISABLE_FONT_FALLBACK
                    isFullFallback = false;
                    Logger::warning("switch: saving RAM: full font fallback is disabled!");
                #endif

                if (isFullFallback)
                    Logger::info("switch: non applet mode, all shared fonts will be loaded!");
                else
                    Logger::info("switch: applet mode or disabling fallback, only shared font for current locale will be loaded.");

                if (locale == LOCALE_ZH_CN || locale == LOCALE_ZH_HANS || isFullFallback)
                {
                    // S.Chinese
                    Application::loadFontFromMemory(FONT_HOS_SCHINESE,
                        fontsSorted[PlSharedFontType_ChineseSimplified]->address,
                        fontsSorted[PlSharedFontType_ChineseSimplified]->size, false);
                    // Ext S.Chinese
                    Application::loadFontFromMemory(FONT_HOS_SCHINESE_EXTEND,
                        fontsSorted[PlSharedFontType_ExtChineseSimplified]->address,
                        fontsSorted[PlSharedFontType_ExtChineseSimplified]->size, false);
                }

                if (locale == LOCALE_ZH_TW || locale == LOCALE_ZH_HANT || isFullFallback)
                    // T.Chinese
                    Application::loadFontFromMemory(FONT_HOS_TCHINESE,
                        fontsSorted[PlSharedFontType_ChineseTraditional]->address,
                        fontsSorted[PlSharedFontType_ChineseTraditional]->size, false);

                if (locale == LOCALE_KO || isFullFallback)
                    // Korean
                    Application::loadFontFromMemory(FONT_HOS_KOREAN,
                        fontsSorted[PlSharedFontType_KO]->address,
                        fontsSorted[PlSharedFontType_KO]->size, false);
            }

            // Extented (symbols)
            Application::loadFontFromMemory(FONT_SWITCH_ICONS,
                fontsSorted[PlSharedFontType_NintendoExt]->address,
                fontsSorted[PlSharedFontType_NintendoExt]->size, false);
        }
        else
            Logger::error("switch: shared font loading stopped - could not load multi shared font: {:#x}", rc);
    }
    else
        Logger::error("switch: shared font loading stopped - could not load languageCode: {:#x}", rc);

    // Material icons
    if (!this->loadMaterialFromResources())
        Logger::error("switch: could not load Material icons font from resources", rc);
}

bool SwitchFontLoader::loadSharedFontFallbackOrder(SharedFontFallbackOrder* order_list, int order_list_count)
{
    Result rc;
    PlFontData fonts[PlSharedFontType_Total];

    // todo: plGetSharedFont must use (u64)languageCode, conflict with suggested use of (std::string)locale?
    uint64_t languageCode = 0;
    rc = setGetSystemLanguage(&languageCode);
    if (R_SUCCEEDED(rc))
    {
        int total_fonts=0;
        rc = plGetSharedFont(languageCode, fonts, PlSharedFontType_Total, &total_fonts);
        if (R_SUCCEEDED(rc))
        {
            for (int i=0; i<order_list_count; i++)
            {
                order_list->push_back(fonts[i].type);
                Logger::debug("switch: pushing type {:#x} to font fallback order list", order_list->at(i));
            }
        }
        return true;
    }
    return false;
}

} // namespace brls
