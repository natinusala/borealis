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

HorizonLightTheme::HorizonLightTheme()
{
    this->backgroundColor[0] = 0.922f;
    this->backgroundColor[1] = 0.922f;
    this->backgroundColor[2] = 0.922f;
    this->backgroundColorRGB = nvgRGB(235, 235, 235);

    this->textColor        = nvgRGB(51, 51, 51);
    this->descriptionColor = nvgRGB(140, 140, 140);

    this->notificationTextColor = nvgRGB(255, 255, 255);
    this->backdropColor         = nvgRGBA(0, 0, 0, 178);

    this->separatorColor = nvgRGB(45, 45, 45);

    this->sidebarColor          = nvgRGB(240, 240, 240);
    this->activeTabColor        = nvgRGB(49, 79, 235);
    this->sidebarSeparatorColor = nvgRGB(208, 208, 208);

    this->highlightBackgroundColor = nvgRGB(252, 255, 248);
    this->highlightColor1          = nvgRGB(13, 182, 213);
    this->highlightColor2          = nvgRGB(80, 239, 217);

    this->listItemSeparatorColor  = nvgRGB(207, 207, 207);
    this->listItemValueColor      = nvgRGB(43, 81, 226);
    this->listItemFaintValueColor = nvgRGB(181, 184, 191);

    this->tableEvenBackgroundColor = nvgRGB(240, 240, 240);
    this->tableBodyTextColor       = nvgRGB(131, 131, 131);

    this->dropdownBackgroundColor = nvgRGBA(0, 0, 0, 178);

    this->nextStageBulletColor = nvgRGB(165, 165, 165);

    this->spinnerBarColor = nvgRGBA(131, 131, 131, 102);

    this->headerRectangleColor = nvgRGB(127, 127, 127);

    this->buttonPlainEnabledBackgroundColor  = nvgRGB(50, 79, 241);
    this->buttonPlainDisabledBackgroundColor = nvgRGB(201, 201, 209);
    this->buttonPlainEnabledTextColor        = nvgRGB(255, 255, 255);
    this->buttonPlainDisabledTextColor       = nvgRGB(220, 220, 228);

    this->dialogColor                = nvgRGB(240, 240, 240);
    this->dialogBackdrop             = nvgRGBA(0, 0, 0, 100);
    this->dialogButtonColor          = nvgRGB(46, 78, 255);
    this->dialogButtonSeparatorColor = nvgRGB(210, 210, 210);
}

HorizonDarkTheme::HorizonDarkTheme()
{
    this->backgroundColor[0] = 0.176f;
    this->backgroundColor[1] = 0.176f;
    this->backgroundColor[2] = 0.176f;
    this->backgroundColorRGB = nvgRGB(45, 45, 45);

    this->textColor        = nvgRGB(255, 255, 255);
    this->descriptionColor = nvgRGB(163, 163, 163);

    this->notificationTextColor = nvgRGB(255, 255, 255);
    this->backdropColor         = nvgRGBA(0, 0, 0, 178);

    this->separatorColor = nvgRGB(255, 255, 255);

    this->sidebarColor          = nvgRGB(50, 50, 50);
    this->activeTabColor        = nvgRGB(0, 255, 204);
    this->sidebarSeparatorColor = nvgRGB(81, 81, 81);

    this->highlightBackgroundColor = nvgRGB(31, 34, 39);
    this->highlightColor1          = nvgRGB(25, 138, 198);
    this->highlightColor2          = nvgRGB(137, 241, 242);

    this->listItemSeparatorColor  = nvgRGB(78, 78, 78);
    this->listItemValueColor      = nvgRGB(88, 195, 169);
    this->listItemFaintValueColor = nvgRGB(93, 103, 105);

    this->tableEvenBackgroundColor = nvgRGB(57, 58, 60);
    this->tableBodyTextColor       = nvgRGB(155, 157, 156);

    this->dropdownBackgroundColor = nvgRGBA(0, 0, 0, 178); // TODO: 178 may be too much for dark theme

    this->nextStageBulletColor = nvgRGB(165, 165, 165);

    this->spinnerBarColor = nvgRGBA(131, 131, 131, 102); // TODO: get this right

    this->headerRectangleColor = nvgRGB(160, 160, 160);

    this->buttonPlainEnabledBackgroundColor  = nvgRGB(1, 255, 201);
    this->buttonPlainDisabledBackgroundColor = nvgRGB(83, 87, 86);
    this->buttonPlainEnabledTextColor        = nvgRGB(52, 41, 55);
    this->buttonPlainDisabledTextColor       = nvgRGB(71, 75, 74);

    this->dialogColor                = nvgRGB(70, 70, 70);
    this->dialogBackdrop             = nvgRGBA(0, 0, 0, 100);
    this->dialogButtonColor          = nvgRGB(3, 251, 199);
    this->dialogButtonSeparatorColor = nvgRGB(103, 103, 103);
}

} // namespace brls
