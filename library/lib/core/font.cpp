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

#include <unistd.h>

#include <borealis/core/application.hpp>
#include <borealis/core/assets.hpp>
#include <borealis/core/font.hpp>

#define MATERIAL_ICONS_PATH BRLS_ASSET("material/MaterialIcons-Regular.ttf")

namespace brls
{

bool FontLoader::loadFontFromFile(std::string fontName, std::string filePath)
{
    if (access(filePath.c_str(), F_OK) != -1)
    {
        bool loaded = Application::loadFontFromFile(fontName, filePath);

        if (!loaded)
        {
            Logger::error("{} font was located but couldn't be loaded", fontName);
            return false;
        }

        return true;
    }

    Logger::error("{} font couldn't be located (searched at \"{}\")", fontName, filePath);
    return false;
}

bool FontLoader::loadMaterialFromResources()
{
    return this->loadFontFromFile(FONT_MATERIAL_ICONS, MATERIAL_ICONS_PATH);
}

} // namespace brls
