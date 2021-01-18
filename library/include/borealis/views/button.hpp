/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2021  natinusala
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

#pragma once

#include <borealis/views/box.hpp>
#include <borealis/views/label.hpp>

namespace brls
{

// Style and colors of different buttons styles
// Border color entries can be empty if thickness is 0
// Highlight padding can be empty
// Background color entries can be empty
// Border thickness can be empty
// The rest is mandatory
typedef struct ButtonStyle
{
    // Regular values
    ShadowType shadowType;
    bool hideHighlightBackground;

    // Style entries
    std::string highlightPadding;
    std::string borderThickness;

    // Theme entries
    std::string enabledBackgroundColor;
    std::string enabledLabelColor;
    std::string enabledBorderColor;

    std::string disabledBackgroundColor;
    std::string disabledLabelColor;
    std::string disabledBorderColor;
} ButtonStyle;

// primary action button (different background color than default, to catch the eye)
inline const ButtonStyle BUTTONSTYLE_PRIMARY = {
    shadowType : ShadowType::GENERIC,
    hideHighlightBackground : true,

    highlightPadding : "brls/button/primary_highlight_padding",
    borderThickness : "",

    enabledBackgroundColor : "brls/button/primary_enabled_background",
    enabledLabelColor : "brls/button/primary_enabled_text",
    enabledBorderColor : "",

    disabledBackgroundColor : "brls/button/primary_disabled_background",
    disabledLabelColor : "brls/button/primary_disabled_text",
    disabledBorderColor : "",
};

// between primary and default - text color is different, background color is the same
inline const ButtonStyle BUTTONSTYLE_HIGHLIGHT = {
    shadowType : ShadowType::GENERIC,
    hideHighlightBackground : true,

    highlightPadding : "",
    borderThickness : "",

    enabledBackgroundColor : "brls/button/default_enabled_background",
    enabledLabelColor : "brls/button/highlight_enabled_text",
    enabledBorderColor : "",

    disabledBackgroundColor : "brls/button/default_disabled_background",
    disabledLabelColor : "brls/button/highlight_disabled_text",
    disabledBorderColor : "",
};

// default, plain button
inline const ButtonStyle BUTTONSTYLE_DEFAULT = {
    shadowType : ShadowType::GENERIC,
    hideHighlightBackground : true,

    highlightPadding : "",
    borderThickness : "",

    enabledBackgroundColor : "brls/button/default_enabled_background",
    enabledLabelColor : "brls/button/default_enabled_text",
    enabledBorderColor : "",

    disabledBackgroundColor : "brls/button/default_disabled_background",
    disabledLabelColor : "brls/button/default_disabled_text",
    disabledBorderColor : "",
};

// text and a border
inline const ButtonStyle BUTTONSTYLE_BORDERED = {
    shadowType : ShadowType::NONE,
    hideHighlightBackground : false,

    highlightPadding : "",
    borderThickness : "brls/button/border_thickness",

    enabledBackgroundColor : "",
    enabledLabelColor : "brls/button/default_enabled_text",
    enabledBorderColor : "brls/button/enabled_border_color",

    disabledBackgroundColor : "",
    disabledLabelColor : "brls/button/default_disabled_text",
    disabledBorderColor : "brls/button/disabled_border_color",
};

// only text
inline const ButtonStyle BUTTONSTYLE_BORDERLESS = {
    shadowType : ShadowType::NONE,
    hideHighlightBackground : false,

    highlightPadding : "",
    borderThickness : "",

    enabledBackgroundColor : "",
    enabledLabelColor : "brls/button/default_enabled_text",
    enabledBorderColor : "",

    disabledBackgroundColor : "",
    disabledLabelColor : "brls/button/default_disabled_text",
    disabledBorderColor : "",
};

enum class ButtonState
{
    ENABLED = 0, // the user can select and click on the button
    DISABLED, // the user can select but not click on the button (greyed out)
};

// A button
class Button : public Box
{
  public:
    Button();

    void onFocusGained() override;
    void onFocusLost() override;

    static View* create();

    /**
     * Sets the style of the button. can be a pointer to one of the
     * BUTTONSTYLE constants or any other user created style.
     */
    void setStyle(const ButtonStyle* style);

    /**
     * Sets the state of the button.
     */
    void setState(ButtonState state);

  private:
    const ButtonStyle* style = &BUTTONSTYLE_DEFAULT;
    ButtonState state        = ButtonState::ENABLED;

    void applyStyle();

    Label* label;
};

} // namespace brls
