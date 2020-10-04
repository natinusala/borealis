/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  WerWolv
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

#include <borealis/style.hpp>

namespace brls
{

HorizonStyle::HorizonStyle()
{
    this->AppletFrame = {
        .headerHeightRegular = 88,
        .headerHeightPopup   = 129,
        .footerHeight        = 73,

        .imageLeftPadding = 64,
        .imageTopPadding  = 20,
        .imageSize        = 52,
        .separatorSpacing = 30,

        .titleSize   = 28,
        .titleStart  = 130,
        .titleOffset = 5,

        .footerTextSize    = 22,
        .footerTextSpacing = 30,

        .slideAnimation = 20
    };

    this->Highlight = {
        .strokeWidth  = 5,
        .cornerRadius = 0.5f,

        .shadowWidth   = 2,
        .shadowOffset  = 10,
        .shadowFeather = 10,
        .shadowOpacity = 128
    };

    this->Background = {
        .sidebarBorderHeight = 16
    };

    this->Sidebar = {
        .width   = 410,
        .spacing = 0,

        .marginLeft   = 88,
        .marginRight  = 30,
        .marginTop    = 40,
        .marginBottom = 40,

        .Item = {
            .height   = 70,
            .textSize = 22,
            .padding  = 9,

            .textOffsetX       = 14,
            .activeMarkerWidth = 4,
        },

        .Separator = { .height = 28 }
    };

    this->List = {
        .marginLeftRight = 60,
        .marginTopBottom = 42,
        .spacing         = 61,

        .Item = {
            .height             = 69, // offset by 1 to have the highlight hide the separator
            .heightWithSubLabel = 99,
            .valueSize          = 20,
            .padding            = 15,
            .thumbnailPadding   = 11,

            .descriptionIndent  = 20,
            .descriptionSpacing = 16,

            .indent = 40,

            .selectRadius = 15 }
    };

    this->Label = {
        .regularFontSize      = 20,
        .mediumFontSize       = 18,
        .smallFontSize        = 16,
        .descriptionFontSize  = 16,
        .crashFontSize        = 24,
        .buttonFontSize       = 24,
        .listItemFontSize     = 24,
        .notificationFontSize = 18,
        .dialogFontSize       = 24,
        .hintFontSize         = 22,

        .lineHeight             = 1.65f,
        .notificationLineHeight = 1.35f
    };

    this->CrashFrame = {
        .labelWidth     = 0.60f,
        .boxStrokeWidth = 5,
        .boxSize        = 64,
        .boxSpacing     = 90,
        .buttonWidth    = 356,
        .buttonHeight   = 60,
        .buttonSpacing  = 47
    };

    this->Button = {
        .cornerRadius = 5.0f,

        .highlightInset = 2,

        .shadowWidth   = 2.0f,
        .shadowFeather = 10.0f,
        .shadowOpacity = 63.75f,
        .shadowOffset  = 10.0f
    };

    this->TableRow = {
        .headerHeight   = 60,
        .headerTextSize = 22,

        .bodyHeight   = 38,
        .bodyIndent   = 40,
        .bodyTextSize = 18,

        .padding = 15
    };

    this->Dropdown = {
        .listWidth   = 720,
        .listPadding = 40,

        .listItemHeight   = 60,
        .listItemTextSize = 20,

        .headerHeight   = 71,
        .headerFontSize = 24,
        .headerPadding  = 70
    };

    this->PopupFrame = {
        .edgePadding      = 120,
        .separatorSpacing = 30,
        .footerHeight     = 73,
        .imageLeftPadding = 60,
        .imageTopPadding  = 17,
        .imageSize        = 100,
        .contentWidth     = 1040,
        .contentHeight    = 518,

        .headerTextLeftPadding = 180,
        .headerTextTopPadding  = 64,

        .subTitleLeftPadding = 182,
        .subTitleTopPadding  = 95,
        .subTitleSpacing     = 20,

        .subTitleSeparatorLeftPadding = 280,
        .subTitleSeparatorTopPadding  = 92,
        .subTitleSeparatorHeight      = 20,

        .headerFontSize   = 28,
        .subTitleFontSize = 16
    };

    this->StagedAppletFrame = {
        .progressIndicatorSpacing          = 4,
        .progressIndicatorRadiusUnselected = 5 - 1, // minus half of border width
        .progressIndicatorRadiusSelected   = 8,
        .progressIndicatorBorderWidth      = 2
    };

    this->ProgressSpinner = {
        .centerGapMultiplier = 0.2f,
        .barWidthMultiplier  = 0.06f
    };

    this->ProgressDisplay = {
        .percentageLabelWidth = 70
    };

    this->Header = {
        .height  = 44,
        .padding = 11,

        .rectangleWidth = 5,

        .fontSize = 18
    };

    this->FramerateCounter = {
        .width  = 125,
        .height = 26
    };

    this->ThumbnailSidebar = {
        .marginLeftRight = 109, // used for the image only = (410 - 192) / 2, image size is 192*192 with a 410px wide sidebar
        .marginTopBottom = 47,

        .buttonHeight = 70,
        .buttonMargin = 60
    };

    this->AnimationDuration = {
        .show      = 250,
        .showSlide = 125,

        .highlight = 100,
        .shake     = 15,

        .collapse = 100,

        .progress = 1000,

        .notificationTimeout = 4000
    };

    this->Notification = {
        .width   = 280,
        .padding = 16,

        .slideAnimation = 40
    };

    this->Dialog = {
        .width  = 770,
        .height = 220,

        .paddingTopBottom = 65,
        .paddingLeftRight = 115,

        .cornerRadius = 5.0f,

        .buttonHeight          = 72,
        .buttonSeparatorHeight = 2,

        .shadowWidth   = 2.0f,
        .shadowFeather = 10.0f,
        .shadowOpacity = 63.75f,
        .shadowOffset  = 10.0f
    };
}

} // namespace brls
