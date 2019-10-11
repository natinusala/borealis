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

#include <borealis/application.hpp>
#include <borealis/button.hpp>

namespace brls
{

Button::Button(ButtonStyle style, std::string text)
    : style(style)
{
    this->label = new Label(this->getLabelStyle(), text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);
}

LabelStyle Button::getLabelStyle()
{
    if (this->style == ButtonStyle::BORDERLESS)
        return LabelStyle::BUTTON_BORDERLESS;
    else if (this->style == ButtonStyle::DIALOG)
        return LabelStyle::BUTTON_DIALOG;

    if (this->state == ButtonState::DISABLED)
        return LabelStyle::BUTTON_PLAIN_DISABLED;
    else
        return LabelStyle::BUTTON_PLAIN;
}

Button::~Button()
{
    delete this->label;
}

void Button::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    this->label->setWidth(this->width);
    this->label->layout(vg, style, stash);
    this->label->setBoundaries(
        this->x,
        this->y + this->height / 2 - this->label->getHeight() / 2,
        this->label->getWidth(),
        this->label->getHeight());
    this->label->invalidate();
}

void Button::setState(ButtonState state)
{
    this->state = state;
    this->label->setStyle(this->getLabelStyle());
}

ButtonState Button::getState()
{
    return this->state;
}

void Button::getHighlightInsets(unsigned* top, unsigned* right, unsigned* bottom, unsigned* left)
{
    if (this->style == ButtonStyle::DIALOG)
    {
        View::getHighlightInsets(top, right, bottom, left);
        *right -= 1;
        return;
    }

    Style* style = Application::getStyle();
    *top         = style->Button.highlightInset;
    *right       = style->Button.highlightInset;
    *bottom      = style->Button.highlightInset;
    *left        = style->Button.highlightInset;
}

void Button::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    float cornerRadius = (float)style->Button.cornerRadius;

    // Background
    switch (this->style)
    {
        case ButtonStyle::PLAIN:
        {
            nvgFillColor(vg, a(this->state == ButtonState::DISABLED ? ctx->theme->buttonPlainDisabledBackgroundColor : ctx->theme->buttonPlainEnabledBackgroundColor));
            nvgBeginPath(vg);
            nvgRoundedRect(vg, x, y, width, height, cornerRadius);
            nvgFill(vg);
            break;
        }
        default:
            break;
    }

    // Shadow
    if (this->state == ButtonState::ENABLED && this->style == ButtonStyle::PLAIN)
    {
        float shadowWidth   = style->Button.shadowWidth;
        float shadowFeather = style->Button.shadowFeather;
        float shadowOpacity = style->Button.shadowOpacity;
        float shadowOffset  = style->Button.shadowOffset;

        NVGpaint shadowPaint = nvgBoxGradient(vg,
            x, y + shadowWidth,
            width, height,
            cornerRadius * 2, shadowFeather,
            RGBA(0, 0, 0, shadowOpacity * alpha), transparent);

        nvgBeginPath(vg);
        nvgRect(vg, x - shadowOffset, y - shadowOffset,
            width + shadowOffset * 2, height + shadowOffset * 3);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgPathWinding(vg, NVG_HOLE);
        nvgFillPaint(vg, shadowPaint);
        nvgFill(vg);
    }

    // Label
    this->label->frame(ctx);
}

bool Button::onClick()
{
    if (this->state == ButtonState::DISABLED)
        return false;

    if (this->clickListener)
        this->clickListener(this);

    return this->clickListener != nullptr;
}

void Button::setClickListener(EventListener listener)
{
    this->clickListener = listener;
}

} // namespace brls
