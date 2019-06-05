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
    LabelStyle labelStyle;
    //TODO: Add all other styles
    switch (style)
    {
        case ButtonStyle::BORDERED:
        default:
            labelStyle = LabelStyle::BUTTON;
            break;
    }

    this->label = new Label(labelStyle, text);
}

Button::~Button()
{
    delete this->label;
}

//TODO: Change look of highlight
void Button::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    //TODO: Draw it
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