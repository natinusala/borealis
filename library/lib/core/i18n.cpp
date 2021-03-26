/*
    Copyright 2020-2021 natinusala
    Copyright 2021 EmreTech

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

#include <borealis/core/application.hpp>
#include <borealis/core/assets.hpp>
#include <borealis/core/i18n.hpp>
#include <filesystem>
#include <tinyxml2/tinyxml2.h>
#include <unordered_map>

namespace brls
{

typedef std::unordered_map<std::string, std::string> locales;

const char *internalXML = 
    "<brls:i18nDoc>"
    "   <brls:List name=\"hints\">"
    "       <brls:String name=\"ok\">OK</brls:String>"
    "       <brls:String name=\"back\">Back</brls:String>"
    "       <brls:String name=\"exit\">Exit</brls:String>"
    "   </brls:List>"
    "   <brls:List name=\"crash_frame\">"
    "       <brls:String name=\"button\">OK</brls:String>"
    "   </brls:List>"
    "   <brls:List name=\"thumbnail_sidebar\">"
    "       <brls:String name=\"save\">Save</brls:String>"
    "   </brls:List>"
    "</brls:i18nDoc>";

static locales xmlDefaultLocale;
static locales xmlCurrentLocale;
static locales xmlInternalText;

void getTextFromElements(tinyxml2::XMLElement *root, std::string existing_path, locales &target)
{
    if (!root) {
        Logger::debug("nullptr???");
        return;
    }

    for (tinyxml2::XMLElement *e2 = root->FirstChildElement(); e2 != NULL; e2 = e2->NextSiblingElement())
    {
        std::string path = existing_path + std::string("/") + e2->Attribute("name");
        if (std::strcmp(e2->Name(), "brls:List") == 0) 
            getTextFromElements(e2, path, target);
        else if (std::strcmp(e2->Name(), "brls:String") == 0)
        {
            tinyxml2::XMLText *textFromElem = e2->FirstChild()->ToText();
            target[path] = textFromElem->Value();
        } 
    }
}

static void loadLocale(std::string locale, locales &target)
{
    std::string localePath = BRLS_ASSET("i18n/" + locale);

    if (!std::filesystem::exists(localePath))
    {
        Logger::error("Cannot load locale {}: directory {} doesn't exist", locale, localePath);
        return;
    }
    else if (!std::filesystem::is_directory(localePath))
    {
        Logger::error("Cannot load locale {}: {} isn't a directory", locale, localePath);
        return;
    }

    // Iterate over all XML files in the directory
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(localePath))
    {
        if (entry.is_directory())
            continue;

        std::string name = entry.path().filename().string();
        std::string extension = entry.path().filename().extension().string();

        if (!(extension == ".xml"))
            continue;

        std::string path = entry.path().string();

        tinyxml2::XMLDocument doc; 
        tinyxml2::XMLError error = doc.LoadFile(path.c_str());

        if (error != tinyxml2::XML_SUCCESS) 
        {
            Logger::error("Cannot load file {} found in locale {}: error code {}", entry.path().filename().string(), locale, std::to_string(error));
            Logger::error("More details: {}", doc.ErrorStr());
            return;
        }

        tinyxml2::XMLElement *root = doc.RootElement();
        if (std::strcmp(root->Name(), "brls:i18nDoc") != 0)
        {
            Logger::error("Could not find root element with name \"brls:i18nDoc\".");
            return;
        }
        
        // Iterate over all XML elements in the file
        std::string path_2 = name.substr(0, name.find("."));
        getTextFromElements(root, path_2, target);
    }
}

void loadInternal()
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.Parse(internalXML);

    if (error != tinyxml2::XML_SUCCESS)
    {
        Logger::error("Cannot load internal XML: error code {}", std::to_string(error));
        Logger::error("More details: {}", doc.ErrorStr());
        return;
    }

    tinyxml2::XMLElement *root = doc.RootElement();
        
    // Iterate over all XML elements in the file
    getTextFromElements(root, "brls", xmlInternalText);
}

void loadTranslations()
{
    loadInternal();
    loadLocale(LOCALE_DEFAULT, xmlDefaultLocale);

    std::string currentLocaleName = Application::getLocale();
    if (currentLocaleName != LOCALE_DEFAULT)
        loadLocale(currentLocaleName, xmlCurrentLocale);
}

namespace internal
{
    std::string getRawStr(std::string stringName)
    {
        std::string currentLocaleName = Application::getLocale();
        
        // First look for translated string in current locale
        if (currentLocaleName != LOCALE_DEFAULT)
        {
            for (auto& [path, value] : xmlCurrentLocale)
            {
                if (stringName == path)
                    return value;
            }
        }

        // Then look for default locale
        else if (currentLocaleName == LOCALE_DEFAULT)
        {
            for (auto& [path, value] : xmlDefaultLocale)
            {
                if (stringName == path)
                    return value;
            }
        }

        // Fallback to returning the string name
        return stringName;
    }

    std::string getInternalRawStr(std::string stringName)
    {
        for (auto& [path, value] : xmlInternalText)
        {
            if (stringName == path)
                return value;
        }

        // Fallback to returning the string name
        return stringName;
    }
} // namespace internal

inline namespace literals
{
    std::string operator"" _i18n(const char* str, size_t len)
    {
        return internal::getRawStr(std::string(str, len));
    }

    std::string operator"" _internal(const char* str, size_t len)
    {
        return internal::getInternalRawStr(std::string(str, len));
    } 
} // namespace literals

} // namespace brls
