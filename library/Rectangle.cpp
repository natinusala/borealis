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

#include <Rectangle.hpp>

Rectangle::Rectangle(NVGcolor color)
{
    this->setColor(color);
}

void Rectangle::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    nvgFillColor(vg, a(this->color));

    nvgBeginPath(vg);
    nvgRect(vg, x, y, width, height);
    nvgFill(vg);
}

void Rectangle::setColor(NVGcolor color)
{
    this->color = color;
}

void Rectangle::layout(NVGcontext* vg, Style *style)
{
    // Nothing to do
}