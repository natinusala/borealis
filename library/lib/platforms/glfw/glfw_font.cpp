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
#include <borealis/platforms/glfw/glfw_font.hpp>

#define USER_REGULAR_PATH BRLS_ASSET("User-Regular.ttf")
#define INTER_FONT_PATH BRLS_ASSET("inter/Inter-Switch.ttf")

#define USER_SWITCH_ICONS_PATH BRLS_ASSET("User-Switch-Icons.ttf")

namespace brls
{

void GLFWFontLoader::loadFonts()
{
    // Regular
    // Try to use user-provided font first, fallback to Inter
    if (access(USER_REGULAR_PATH, F_OK) != -1)
        this->loadFontFromFile(FONT_REGULAR, USER_REGULAR_PATH);
    else
        this->loadFontFromFile(FONT_REGULAR, INTER_FONT_PATH);

    // Korean unimplemented

    // Switch icons
    // Only supports user-provided font
    this->loadFontFromFile(FONT_SWITCH_ICONS, USER_SWITCH_ICONS_PATH);

    // Material icons
    this->loadMaterialFromResources();
}

} // namespace brls
