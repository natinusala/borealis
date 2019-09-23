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

#include <borealis/button.hpp>
#include <borealis/application.hpp>

namespace brls
{

// TODO: Add a DISABLED state

Button::Button(ButtonStyle style, std::string text)
    : style(style)
{
    this->label = new Label(this->getLabelStyle(), text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);
}

LabelStyle Button::getLabelStyle()
{
    switch (this->style)
    {
        case ButtonStyle::CRASH:
            return LabelStyle::CRASH_BUTTON;
        default:
            return LabelStyle::BUTTON;
    }
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
}

void Button::getHighlightInsets(unsigned* top, unsigned* right, unsigned* bottom, unsigned* left)
{
    Style* style = Application::getStyle();
    *top    = style->Button.highlightInset;
    *right  = style->Button.highlightInset;
    *bottom = style->Button.highlightInset;
    *left   = style->Button.highlightInset;
}

void Button::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    // Background
    switch (this->style)
    {
        case ButtonStyle::PLAIN:
        {
            nvgFillColor(vg, a(ctx->theme->buttonPlainEnabledBackgroundColor));
            nvgRoundedRect(vg, x, y, width, height, style->Button.cornerRadius);
            nvgFill(vg);
            break;
        }
        default:
            break;
    }

    // TODO: Shadow (for plain style)

    // Label
    this->label->frame(ctx);
}

bool Button::onClick()
{
    if (this->clickListener)
        this->clickListener(this);

    return this->clickListener != nullptr;
}

void Button::setClickListener(EventListener listener)
{
    this->clickListener = listener;
}

} // namespace brls
