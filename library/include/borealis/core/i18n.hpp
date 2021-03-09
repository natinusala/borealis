/*
    Copyright 2020-2021 natinusala

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

#include <fmt/core.h>

#include <borealis/core/logger.hpp>
#include <string>

namespace brls
{

const std::string LOCALE_JA      = "ja";
const std::string LOCALE_EN_US   = "en-US";
const std::string LOCALE_EN_GB   = "en-GB";
const std::string LOCALE_FR      = "fr";
const std::string LOCALE_FR_CA   = "fr-CA";
const std::string LOCALE_DE      = "de";
const std::string LOCALE_IT      = "it";
const std::string LOCALE_ES      = "es";
const std::string LOCALE_ZH_CN   = "zh-CN";
const std::string LOCALE_ZH_HANS = "zh-Hans";
const std::string LOCALE_ZH_HANT = "zh-Hant";
const std::string LOCALE_ZH_TW   = "zh-TW";
const std::string LOCALE_Ko      = "ko";
const std::string LOCALE_NL      = "nl";
const std::string LOCALE_PT      = "pt";
const std::string LOCALE_PT_BR   = "pt-BR";
const std::string LOCALE_RU      = "ru";
const std::string LOCALE_ES_419  = "es-419";

const std::string LOCALE_DEFAULT = LOCALE_EN_US;

namespace internal
{
    std::string getRawStr(std::string stringName);
} // namespace internal

/**
 * Returns the translation for the given string,
 * after injecting format parameters (if any)
 */
template <typename... Args>
std::string getStr(std::string stringName, Args&&... args)
{
    std::string rawStr = internal::getRawStr(stringName);

    try
    {
        return fmt::format(rawStr, args...);
    }
    catch (const std::exception& e)
    {
        Logger::error("Invalid format \"{}\" from string \"{}\": {}", rawStr, stringName, e.what());
        return stringName;
    }
}

/**
 * Loads all translations of the current system locale + default locale
 * Must be called before trying to get a translation!
 */
void loadTranslations();

inline namespace literals
{
    /**
     * Returns the translation for the given string, without
     * injecting any parameters
     * Shortcut to brls::getStr(stringName)
     */
    std::string operator"" _i18n(const char* str, size_t len);
} // namespace literals
} // namespace brls
