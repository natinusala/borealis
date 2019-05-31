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

#include <Style.hpp>

static Style styleAccurate = {
    .SettingsFrame = {
        .headerHeight      = 88,
        .footerHeight      = 73,

        .separatorSpacing  = 30,

        .titleSize      = 28,
        .titleStart     = 130,
        .titleOffset    = 5
    },

    .Highlight = {
        .strokeWidth    = 5,
        .cornerRadius   = 0.5f,

        .shadowWidth    = 2,
        .shadowOffset   = 10,
        .shadowFeather  = 10,
        .shadowOpacity  = 128
    },

    .Background = {
        .sidebarBorderHeight = 16
    },

    .Sidebar = {
        .width      = 410,
        .spacing    = 0,

        .marginLeft     = 88,
        .marginRight    = 30,
        .marginTop      = 40,
        .marginBottom   = 40,

        .Item = {
            .height     = 70,
            .textSize   = 22,
            .padding    = 9,

            .textOffsetX        = 14,
            .activeMarkerWidth  = 4,
        },

        .Separator = {
            .height = 28
        }
    },

    .List = {
        .marginLeftRight    = 60,
        .marginTopBottom    = 42,
        .spacing            = 55,

        .Item = {
            .height             = 69, // offset by 1 to have the highlight hide the separator
            .textSize           = 24,
            .valueSize          = 20,
            .padding            = 15,
            .sublabelIndent     = 20,
            .sublabelSpacing    = 16
        }
    },

    .Label = {
        .regularFontSize    = 20,
        .sublabelFontSize   = 16
    }
};

static Style *currentStyle = &styleAccurate;

void setStyle(StyleEnum style)
{
    switch (style)
    {
        case STYLE_ACCURATE:
            currentStyle = &styleAccurate;
    }
}

Style *getStyle()
{
    return currentStyle;
}