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
    // AppletFrame
    struct
    {
        unsigned headerHeightRegular;
        unsigned headerHeightLarge;
        unsigned footerHeight;

        unsigned separatorSpacing;

        unsigned titleSize;
        unsigned titleStart;
        unsigned titleOffset;

        unsigned footerTextSize;
        unsigned footerHintSize;
        unsigned footerTextSpacing;
    } AppletFrame;


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

    // Background
    struct
    {
        unsigned sidebarBorderHeight;
    } Background;

    // Sidebar
    struct
    {
        unsigned width;
        unsigned spacing;

        unsigned marginLeft;
        unsigned marginRight;
        unsigned marginTop;
        unsigned marginBottom;

        struct
        {
            unsigned height;
            unsigned textSize;
            unsigned padding;

            unsigned textOffsetX;
            unsigned activeMarkerWidth;

            unsigned highlight;
        } Item;

        struct
        {
            unsigned height;
        } Separator;
    } Sidebar;

    // List
    struct
    {
        unsigned marginLeftRight;
        unsigned marginTopBottom;
        unsigned spacing;

        // Item
        struct
        {
            unsigned height;
            unsigned heightWithSubLabel;
            unsigned valueSize;
            unsigned padding;
            unsigned thumbnailPadding;

            unsigned descriptionIndent;
            unsigned descriptionSpacing;

            unsigned indent;

            unsigned selectRadius;
        } Item;
    } List;

    // Label
    struct
    {
        unsigned regularFontSize;
        unsigned mediumFontSize;
        unsigned smallFontSize;
        unsigned descriptionFontSize;
        unsigned crashFontSize;
        unsigned buttonFontSize;
        unsigned listItemFontSize;
        float lineHeight;
    } Label;

    // CrashFrame
    struct
    {
        float labelWidth; // proportional to frame width, from 0 to 1
        unsigned boxStrokeWidth;
        unsigned boxSize;
        unsigned boxSpacing;
        unsigned buttonWidth;
        unsigned buttonHeight;
        unsigned buttonSpacing;
    } CrashFrame;

    // Button
    struct
    {
        float cornerRadius;
        float crashCornerRadius;
    } Button;

    // TableRow
    struct
    {
        unsigned headerHeight;
        unsigned headerTextSize;

        unsigned bodyHeight;
        unsigned bodyIndent;
        unsigned bodyTextSize;

        unsigned padding;
    } TableRow;

    // Dropdown
    struct
    {
        unsigned listWidth;
        unsigned listPadding;

        unsigned listItemHeight;
        unsigned listItemTextSize;

        unsigned headerHeight;
        unsigned headerFontSize;
        unsigned headerPadding;
    } Dropdown;

    // StagedAppletFrame
    struct
    {
        unsigned progressIndicatorSpacing;
        unsigned progressIndicatorRadiusUnselected;
        unsigned progressIndicatorRadiusSelected;
        unsigned progressIndicatorBorderWidth;
    } StagedAppletFrame;

    // ProgressSpinner
    struct
    {
        float centerGapMultiplier;
        float barWidthMultiplier;
    } ProgressSpinner;

    // ProgressDisplay
    struct
    {
        unsigned percentageLabelWidth;
    } ProgressDisplay;

    // Header
    struct
    {
        unsigned height;
        unsigned padding;

        unsigned rectangleWidth;

        unsigned fontSize;
    } Header;

    // FramerateCounter
    struct
    {
        unsigned width;
        unsigned height;
    } FramerateCounter;
} Style;

// TODO: Make a condensed style
enum class StyleEnum
{
    ACCURATE = 0 // as close to HOS as possible
};

extern Style styleAccurate;
