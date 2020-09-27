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

#pragma once

#include <fmt/core.h>

#include <string>

namespace brls::i18n
{

// TODO: add support for string arrays

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
    std::string rawStr = brls::i18n::internal::getRawStr(stringName);

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

/**
 * Returns the current system locale
 * NOT the one that's currently used in the app!
 */
std::string getCurrentLocale();

inline namespace literals
{
    /**
     * Returns the translation for the given string, without
     * injecting any parameters
     * Shortcut to i18n::getStr(stringName)
     */
    std::string operator"" _i18n(const char* str, size_t len);
} // namespace literals

} // namespace brls::i18n
