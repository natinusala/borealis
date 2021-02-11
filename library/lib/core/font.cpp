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
