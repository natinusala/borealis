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

#include <Button.hpp>

Button::Button(ButtonStyle style, string text) : style(style)
{
    this->label = new Label(LabelStyle::BUTTON, text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);
}

Button::~Button()
{
    delete this->label;
}

void Button::layout(NVGcontext* vg, Style *style, FontStash *stash)
{
    this->label->setWidth(this->width);
    this->label->layout(vg, style, stash);
    this->label->setBoundaries(
        this->x,
        this->y + this->height / 2 - this->label->getHeight() / 2,
        this->label->getWidth(),
        this->label->getHeight()
    );
}

void Button::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    // TODO: The rest
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