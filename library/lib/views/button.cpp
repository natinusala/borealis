/*
    Copyright 2019-2021 natinusala
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

#include <math.h>

#include <borealis/core/application.hpp>
#include <borealis/core/touch/tap_gesture.hpp>
#include <borealis/views/button.hpp>

namespace brls
{

// TODO: soundboard in demo (powered by pulsar)

// TODO: split platform driver entirely: swkbd (move swkbd in switch platform driver folder (hpp too, it doesn't need to be in the regular includes))
// TODO: not a single ifdef __SWITCH__ except from the platform driver selection!

// TODO: image

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

    this->addGestureRecognizer(new TapGestureRecognizer([this](TapGestureRecognizer* recogniser) {
        Application::giveFocus(this);
        for (auto& action : getActions())
        {
            if (action.button != static_cast<enum ControllerButton>(BUTTON_A))
                continue;

            if (action.available)
            {
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
                return false;
            }
        }
        return true;
    },
        false));
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
