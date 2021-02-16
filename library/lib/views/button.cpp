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

#include <math.h>

#include <borealis/core/application.hpp>
#include <borealis/views/button.hpp>
#include <borealis/core/touch/tap_gesture_recognizer.hpp>

namespace brls
{

// TODO: soundboard in demo (powered by pulsar)

// TODO: split platform driver entirely: nvg context, inputs, fonts, swkbd (move swkbd in switch platform driver folder (hpp too, it doesn't need to be in the regular includes))
// TODO: not a single ifdef __SWITCH__ except from the platform driver selection!

// TODO: image
// TODO: click animation

const std::string buttonXML = R"xml(
    <brls:Box
        width="auto"
        height="auto"
        axis="row"
        focusable="true"
        justifyContent="center"
        alignItems="center"
        paddingTop="@style/brls/button/padding_top_bottom"
        paddingRight="@style/brls/button/padding_sides"
        paddingBottom="@style/brls/button/padding_top_bottom"
        paddingLeft="@style/brls/button/padding_sides"
        cornerRadius="@style/brls/button/corner_radius"
        highlightCornerRadius="@style/brls/button/corner_radius">

    <brls:Label
        id="brls/button/label"
        width="auto"
        height="auto"
        fontSize="@style/brls/button/text_size"
        textAlign="center" />

    </brls:Box>
)xml";

Button::Button()
{
    this->inflateFromXMLString(buttonXML);

    this->label = (Label*)this->getView("brls/button/label");

    this->forwardXMLAttribute("text", this->label);
    this->forwardXMLAttribute("singleLine", this->label);
    this->forwardXMLAttribute("fontSize", this->label);
    this->forwardXMLAttribute("textColor", this->label);
    this->forwardXMLAttribute("lineHeight", this->label);
    this->forwardXMLAttribute("animated", this->label);
    this->forwardXMLAttribute("autoAnimate", this->label);
    this->forwardXMLAttribute("textAlign", this->label);

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "style", const ButtonStyle*, this->setStyle,
        {
            { "default", &BUTTONSTYLE_DEFAULT },
            { "primary", &BUTTONSTYLE_PRIMARY },
            { "highlight", &BUTTONSTYLE_HIGHLIGHT },
            { "bordered", &BUTTONSTYLE_BORDERED },
            { "borderless", &BUTTONSTYLE_BORDERLESS },
        });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "state", ButtonState, this->setState,
        {
            { "enabled", ButtonState::ENABLED },
            { "disabled", ButtonState::DISABLED },
        });

    this->applyStyle();

    this->addGestureRecognizer(new TapGestureRecognizer([this](TapGestureRecognizer* recogniser)
    {
        Application::giveFocus(this);
        for (auto& action : getActions())
        {
            if (action.button != static_cast<enum ControllerButton>(BUTTON_A))
                continue;

            if (action.available) {
                this->playClickAnimation(recogniser->getState() != GestureState::UNSURE);

                switch (recogniser->getState()) 
                {
                case GestureState::UNSURE:
                    Application::getAudioPlayer()->play(SOUND_FOCUS_CHANGE);
                    break;
                case GestureState::FAILED:
                case GestureState::INTERRUPTED:
                    Application::getAudioPlayer()->play(SOUND_TOUCH_UNFOCUS);
                    break;
                case GestureState::END:
                    if (action.actionListener(this))
                        Application::getAudioPlayer()->play(action.sound);
                    break;
                }
            }
        }
    }, false));
}

void Button::applyStyle()
{
    Style style = Application::getStyle();
    Theme theme = Application::getTheme();

    this->setShadowType(this->style->shadowType);
    this->setHideHighlightBackground(this->style->hideHighlightBackground);

    if (this->style->highlightPadding != "")
        this->setHighlightPadding(style[this->style->highlightPadding]);
    else
        this->setHighlightPadding(0.0f);

    if (this->style->borderThickness != "")
        this->setBorderThickness(style[this->style->borderThickness]);
    else
        this->setBorderThickness(0.0f);

    std::string backgroundColor;
    std::string textColor;
    std::string borderColor;

    switch (this->state)
    {
        case ButtonState::ENABLED:
            backgroundColor = this->style->enabledBackgroundColor;
            textColor       = this->style->enabledLabelColor;
            borderColor     = this->style->enabledBorderColor;

            break;
        case ButtonState::DISABLED:
            backgroundColor = this->style->disabledBackgroundColor;
            textColor       = this->style->disabledLabelColor;
            borderColor     = this->style->disabledBorderColor;

            break;
    }

    if (backgroundColor != "")
        this->setBackgroundColor(theme[backgroundColor]);
    else
        this->setBackground(ViewBackground::NONE);

    this->label->setTextColor(theme[textColor]);

    if (this->getBorderThickness() > 0.0f)
        this->setBorderColor(theme[borderColor]);
}

void Button::onFocusGained()
{
    Box::onFocusGained();

    this->setShadowVisibility(false);
    // TODO: setBorderVisibility(false);
}

void Button::onFocusLost()
{
    Box::onFocusLost();

    this->setShadowVisibility(true);
    // TODO: setBorderVisibility(true);
}

void Button::setStyle(const ButtonStyle* style)
{
    this->style = style;
    this->applyStyle();
}

void Button::setState(ButtonState state)
{
    this->state = state;
    this->applyStyle();
}

View* Button::create()
{
    return new Button();
}

} // namespace brls
