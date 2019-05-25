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

#pragma once

typedef struct
{
    // SettingsFrame
    struct
    {
        unsigned headerHeight;
        unsigned footerHeight;

        unsigned separatorSpacing;

        unsigned titleSize;
        unsigned titleStart;
    } SettingsFrame;


    // Highlight
    struct
    {
        unsigned strokeWidth;
        float cornerRadius;

        unsigned shadowWidth;
        unsigned shadowOffset;
        unsigned shadowFeather;
        unsigned shadowOpacity;
    } Highlight;
} Style;

// TODO: Make a condensed style
typedef enum
{
    STYLE_ACCURATE = 0 // as close to HOS as possible
} StyleEnum;

/**
 * Sets the global style
 * Must be called before initializing the application
 * Currently cannot be hot swapped
 */
void setStyle(StyleEnum style);

/**
 * Gets the global style
 */
Style *getStyle();