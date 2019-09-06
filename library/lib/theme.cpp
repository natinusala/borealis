/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam

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

#include <borealis/theme.hpp>

namespace brls
{

// Default colors
Theme Theme::horizon()
{
    Theme theme = Theme();

    // Light variant
    theme.colors[ThemeVariant_LIGHT].backgroundColor[0] = 0.922f;
    theme.colors[ThemeVariant_LIGHT].backgroundColor[1] = 0.922f;
    theme.colors[ThemeVariant_LIGHT].backgroundColor[2] = 0.922f;
    theme.colors[ThemeVariant_LIGHT].backgroundColorRGB = nvgRGB(235, 235, 235);

    theme.colors[ThemeVariant_LIGHT].textColor        = nvgRGB(51, 51, 51);
    theme.colors[ThemeVariant_LIGHT].descriptionColor = nvgRGB(140, 140, 140);

    theme.colors[ThemeVariant_LIGHT].separatorColor = nvgRGB(45, 45, 45);

    theme.colors[ThemeVariant_LIGHT].sidebarColor          = nvgRGB(240, 240, 240);
    theme.colors[ThemeVariant_LIGHT].activeTabColor        = nvgRGB(49, 79, 235);
    theme.colors[ThemeVariant_LIGHT].sidebarSeparatorColor = nvgRGB(208, 208, 208);

    theme.colors[ThemeVariant_LIGHT].highlightBackgroundColor = nvgRGB(252, 255, 248);
    theme.colors[ThemeVariant_LIGHT].highlightColor1          = nvgRGB(13, 182, 213);
    theme.colors[ThemeVariant_LIGHT].highlightColor2          = nvgRGB(80, 239, 217);

    theme.colors[ThemeVariant_LIGHT].listItemSeparatorColor  = nvgRGB(207, 207, 207);
    theme.colors[ThemeVariant_LIGHT].listItemValueColor      = nvgRGB(43, 81, 226);
    theme.colors[ThemeVariant_LIGHT].listItemFaintValueColor = nvgRGB(181, 184, 191);

    theme.colors[ThemeVariant_LIGHT].tableEvenBackgroundColor = nvgRGB(240, 240, 240);
    theme.colors[ThemeVariant_LIGHT].tableBodyTextColor       = nvgRGB(131, 131, 131);

    theme.colors[ThemeVariant_LIGHT].dropdownBackgroundColor = nvgRGBA(0, 0, 0, 178);

    theme.colors[ThemeVariant_LIGHT].nextStageBulletColor = nvgRGB(165, 165, 165);

    theme.colors[ThemeVariant_LIGHT].spinnerBarColor = nvgRGBA(131, 131, 131, 102);

    theme.colors[ThemeVariant_LIGHT].headerRectangleColor = nvgRGB(127, 127, 127);

    // Dark variant
    theme.colors[ThemeVariant_DARK].backgroundColor[0] = 0.176f;
    theme.colors[ThemeVariant_DARK].backgroundColor[1] = 0.176f;
    theme.colors[ThemeVariant_DARK].backgroundColor[2] = 0.176f;
    theme.colors[ThemeVariant_DARK].backgroundColorRGB = nvgRGB(45, 45, 45);

    theme.colors[ThemeVariant_DARK].textColor        = nvgRGB(255, 255, 255);
    theme.colors[ThemeVariant_DARK].descriptionColor = nvgRGB(163, 163, 163);

    theme.colors[ThemeVariant_DARK].separatorColor = nvgRGB(255, 255, 255);

    theme.colors[ThemeVariant_DARK].sidebarColor          = nvgRGB(50, 50, 50);
    theme.colors[ThemeVariant_DARK].activeTabColor        = nvgRGB(0, 255, 204);
    theme.colors[ThemeVariant_DARK].sidebarSeparatorColor = nvgRGB(81, 81, 81);

    theme.colors[ThemeVariant_DARK].highlightBackgroundColor = nvgRGB(31, 34, 39);
    theme.colors[ThemeVariant_DARK].highlightColor1          = nvgRGB(25, 138, 198);
    theme.colors[ThemeVariant_DARK].highlightColor2          = nvgRGB(137, 241, 242);

    theme.colors[ThemeVariant_DARK].listItemSeparatorColor  = nvgRGB(78, 78, 78);
    theme.colors[ThemeVariant_DARK].listItemValueColor      = nvgRGB(88, 195, 169);
    theme.colors[ThemeVariant_DARK].listItemFaintValueColor = nvgRGB(93, 103, 105);

    theme.colors[ThemeVariant_DARK].tableEvenBackgroundColor = nvgRGB(57, 58, 60);
    theme.colors[ThemeVariant_DARK].tableBodyTextColor       = nvgRGB(155, 157, 156);

    theme.colors[ThemeVariant_DARK].dropdownBackgroundColor = nvgRGBA(0, 0, 0, 178), // TODO: 178 may be too much for dark theme

        theme.colors[ThemeVariant_DARK].nextStageBulletColor = nvgRGB(165, 165, 165);

    theme.colors[ThemeVariant_DARK].spinnerBarColor = nvgRGBA(131, 131, 131, 102), // TODO: get this right

        theme.colors[ThemeVariant_DARK].headerRectangleColor = nvgRGB(160, 160, 160);

    return theme;
}

} // namespace brls
