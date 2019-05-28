/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

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

#include <Theme.hpp>

Theme themeLight =
{
    .backgroundColor        = { 0.922f, 0.922f, 0.922f },

    .textColor              = nvgRGB(51, 51, 51),
    .sublabelColor          = nvgRGB(140, 140, 140),

    .separatorColor         = nvgRGB(45, 45, 45),

    .sidebarColor           = nvgRGB(240, 240, 240),
    .activeTabColor         = nvgRGB(49, 79, 235),
    .sidebarSeparatorColor  = nvgRGB(208, 208, 208),

    .highlightBackgroundColor   = nvgRGB(252, 255, 248),
    .highlightColor1            = nvgRGB(13, 182, 213),
    .highlightColor2            = nvgRGB(80, 239, 217),

    .listItemSeparatorColor = nvgRGB(207, 207, 207),
    .listItemValueColor     = nvgRGB(43, 81, 226)
};

Theme themeDark =
{
    .backgroundColor        = { 0.176f, 0.176f, 0.176f },

    .textColor              = nvgRGB(255, 255, 255),
    .sublabelColor          = nvgRGB(163, 163, 163),

    .separatorColor         = nvgRGB(255, 255, 255),

    .sidebarColor           = nvgRGB(50, 50, 50),
    .activeTabColor         = nvgRGB(0, 255, 204),
    .sidebarSeparatorColor  = nvgRGB(81, 81, 81),

    .highlightBackgroundColor   = nvgRGB(31, 34, 39),
    .highlightColor1            = nvgRGB(25, 138, 198),
    .highlightColor2            = nvgRGB(137, 241, 242),

    .listItemSeparatorColor = nvgRGB(78, 78, 78),
    .listItemValueColor     = nvgRGB(88, 195, 169)
};