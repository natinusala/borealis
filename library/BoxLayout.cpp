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

#include <BoxLayout.hpp>

#include <stdio.h>

BoxLayout::BoxLayout(BoxLayoutOrientation orientation) : orientation(orientation)
{

}

void BoxLayout::frame(FrameContext *ctx)
{
    // Draw children
    for (BoxLayoutChild *child : this->children)
        child->view->frame(ctx);
}

void BoxLayout::layout()
{
    // Vertical orientation
    if (this->orientation == VERTICAL)
    {
        // TODO
    }
    // Horizontal orientation
    else if (this->orientation == HORIZONTAL)
    {
        unsigned xAdvance = this->x;
        for (BoxLayoutChild *child : this->children)
        {
            if (child->fill)
                child->view->setBoundaries(xAdvance, this->y, this->width - xAdvance, this->height);
            else
                child->view->setBoundaries(xAdvance, this->y, child->view->getWidth(), this->height);

            child->view->layout();

            xAdvance += child->view->getWidth();
        }
    }
}

void BoxLayout::addView(View *view, bool fill)
{
    BoxLayoutChild *child = new BoxLayoutChild();
    child->view     = view;
    child->fill     = fill;

    this->children.push_back(child);
    this->layout();
}