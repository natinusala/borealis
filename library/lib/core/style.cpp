/*
    Copyright 2019 natinusala
    Copyright 2019 WerWolv
    Copyright 2019 p-sam

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

#include <borealis/core/style.hpp>
#include <borealis/core/util.hpp>
#include <stdexcept>

namespace brls
{

static StyleValues styleValues = {
    // Animations
    { "brls/animations/show", 250.0f },
    { "brls/animations/show_slide", 125.0f },

    { "brls/animations/highlight", 100.0f },
    { "brls/animations/highlight_shake", 15.0f },

    { "brls/animations/label_scrolling_timer", 1500.0f },
    { "brls/animations/label_scrolling_speed", 0.05f },

    // Highlight
    { "brls/highlight/stroke_width", 5.0f },
    { "brls/highlight/corner_radius", 0.5f },
    { "brls/highlight/shadow_width", 2.0f },
    { "brls/highlight/shadow_offset", 10.0f },
    { "brls/highlight/shadow_feather", 10.0f },
    { "brls/highlight/shadow_opacity", 128.0f },

    // AppletFrame
    { "brls/applet_frame/padding_sides", 30.0f },

    { "brls/applet_frame/header_height", 88.0f },
    { "brls/applet_frame/header_padding_top_bottom", 15.0f },
    { "brls/applet_frame/header_padding_sides", 35.0f },
    { "brls/applet_frame/header_image_title_spacing", 18.0f },
    { "brls/applet_frame/header_title_font_size", 28.0f },
    { "brls/applet_frame/header_title_top_offset", 7.0f },

    { "brls/applet_frame/footer_height", 73.0f },
    { "brls/applet_frame/footer_padding_top_bottom", 20.0f },
    { "brls/applet_frame/footer_padding_sides", 25.0f },

    // TabFrame
    { "brls/tab_frame/sidebar_width", 410.0f },
    { "brls/tab_frame/content_padding_top_bottom", 42.0f }, // unused by the library, here for users
    { "brls/tab_frame/content_padding_sides", 60.0f }, // unused by the library, here for users

    // Sidebar
    { "brls/sidebar/border_height", 16.0f },
    { "brls/sidebar/padding_top", 32.0f },
    { "brls/sidebar/padding_bottom", 47.0f },
    { "brls/sidebar/padding_left", 80.0f },
    { "brls/sidebar/padding_right", 30.0f },
    { "brls/sidebar/item_height", 70.0f },
    { "brls/sidebar/item_accent_margin_top_bottom", 9.0f },
    { "brls/sidebar/item_accent_margin_sides", 8.0f },
    { "brls/sidebar/item_accent_rect_width", 4.0f },
    { "brls/sidebar/item_font_size", 22.0f },
    { "brls/sidebar/separator_height", 30.0f },

    // Label
    { "brls/label/default_font_size", 20.0f },
    { "brls/label/default_line_height", 1.65f },
    { "brls/label/scrolling_animation_spacing", 50.0f },
    { "brls/label/highlight_padding", 2.0f },

    // Header
    { "brls/header/padding_top_bottom", 11.0f },
    { "brls/header/padding_right", 11.0f },
    { "brls/header/rectangle_width", 5.0f },
    { "brls/header/rectangle_height", 22.0f },
    { "brls/header/rectangle_margin", 10.0f },
    { "brls/header/font_size", 18.0f },

    // Button
    { "brls/button/padding_top_bottom", 15.0f },
    { "brls/button/padding_sides", 25.0f },
    { "brls/button/corner_radius", 5.0f },
    { "brls/button/text_size", 18.0f },
    { "brls/button/primary_highlight_padding", 2.0f },
    { "brls/button/border_thickness", 2.0f },

    // Generic shadow
    { "brls/shadow/width", 2.0f },
    { "brls/shadow/feather", 10.0f },
    { "brls/shadow/opacity", 63.75f },
    { "brls/shadow/offset", 10.0f },
};

static Style style(&styleValues);

Style getStyle()
{
    return style;
}

StyleValues::StyleValues(std::initializer_list<std::pair<std::string, float>> list)
{
    for (std::pair<std::string, float> metric : list)
        this->values.insert(metric);
}

void StyleValues::addMetric(std::string name, float metric)
{
    this->values.insert(std::make_pair(name, metric));
}

float StyleValues::getMetric(std::string name)
{
    if (this->values.count(name) == 0)
        fatal("Unknown style metric \"" + name + "\"");

    return this->values[name];
}

Style::Style(StyleValues* values)
    : values(values)
{
}

float Style::getMetric(std::string name)
{
    return this->values->getMetric(name);
}

void Style::addMetric(std::string name, float metric)
{
    return this->values->addMetric(name, metric);
}

float Style::operator[](std::string name)
{
    return this->getMetric(name);
}

/*
HorizonStyle::HorizonStyle()
{
    this->AppletFrame = {
        .headerHeightRegular = ,
        .headerHeightPopup   = 129,

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
        .shadowOffset  = 10.0f,

        .borderedBorderThickness = 2,
        .regularBorderThickness  = 2
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
*/
} // namespace brls
