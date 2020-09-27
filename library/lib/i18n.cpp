/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  natinusala

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

#include <borealis.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#ifdef __SWITCH__
#include <switch.h>
#endif

#define DEFAULT_LOCALE "en-US"

namespace brls::i18n
{

static nlohmann::json defaultLocale = {};
static nlohmann::json currentLocale = {};

static bool endsWith(const std::string& str, const std::string& suffix)
{
    // if I wanted to write my own endsWith I would have made borealis in PHP
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

static void loadLocale(std::string locale, nlohmann::json* target)
{
    std::string localePath = BOREALIS_ASSET("i18n/" + locale); // TODO: use a platform agnostic separator if someone cares about Windows

    if (!std::filesystem::exists(localePath))
    {
        brls::Logger::error("Cannot load locale {}: directory {} doesn't exist", locale, localePath);
        return;
    }
    else if (!std::filesystem::is_directory(localePath))
    {
        brls::Logger::error("Cannot load locale {}: {} isn't a directory", locale, localePath);
        return;
    }

    // Iterate over all JSON files in the directory
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(localePath))
    {
        if (entry.is_directory())
            continue;

        std::string name { entry.path().filename().u8string() };

        if (!endsWith(name, ".json"))
            continue;

        std::string path { entry.path().u8string() };

        nlohmann::json strings;

        std::ifstream jsonStream;
        jsonStream.open(path);

        try
        {
            jsonStream >> strings;
        }
        catch (const std::exception& e)
        {
            brls::Logger::error("Error while loading \"{}\": {}", path, e.what());
        }

        jsonStream.close();

        (*target)[name.substr(0, name.length() - 5)] = strings;
    }
}

std::string getCurrentLocale()
{
#ifdef __SWITCH__
    u64 languageCode = 0;

    Result res = setGetSystemLanguage(&languageCode);

    if (R_SUCCEEDED(res))
    {
        char* languageName = (char*)&languageCode;
        return std::string(languageName);
    }
    else
    {
        brls::Logger::error("Unable to get system language (error 0x{0:x}), using the default one: {1}", res, DEFAULT_LOCALE);
    }
#endif
    return DEFAULT_LOCALE;
}

void loadTranslations()
{
    loadLocale(DEFAULT_LOCALE, &defaultLocale);

    std::string currentLocaleName = getCurrentLocale();
    if (currentLocaleName != DEFAULT_LOCALE)
        loadLocale(currentLocaleName, &currentLocale);
}

namespace internal
{
    std::string getRawStr(std::string stringName)
    {
        nlohmann::json::json_pointer pointer;

        try
        {
            pointer = nlohmann::json::json_pointer("/" + stringName);
        }
        catch (const std::exception& e)
        {
            brls::Logger::error("Error while getting string \"{}\": {}", stringName, e.what());
            return stringName;
        }

        // First look for translated string in current locale
        try
        {
            return currentLocale[pointer].get<std::string>();
        }
        catch (...)
        {
        }

        // Then look for default locale
        try
        {
            return defaultLocale[pointer].get<std::string>();
        }
        catch (...)
        {
        }

        // Fallback to returning the string name
        return stringName;
    }
} // namespace internal

inline namespace literals
{
    std::string operator"" _i18n(const char* str, size_t len)
    {
        return brls::i18n::internal::getRawStr(std::string(str, len));
    }

} // namespace literals

} // namespace brls::i18n
