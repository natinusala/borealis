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

void BoxLayout::setSpacing(unsigned spacing)
{
    this->spacing = spacing;
    this->layout();
}

void BoxLayout::setMargins(unsigned top, unsigned right, unsigned bottom, unsigned left)
{
    this->marginBottom  = bottom;
    this->marginLeft    = left;
    this->marginRight   = right;
    this->marginTop     = top;
    this->layout();
}

void BoxLayout::layout()
{
    // Vertical orientation
    if (this->orientation == BOXLAYOUT_VERTICAL)
    {
        unsigned yAdvance = this->y + this->marginTop;
        for (BoxLayoutChild *child : this->children)
        {
            unsigned childHeight = child->view->getHeight();

            if (child->fill)
                child->view->setBoundaries(this->x + this->marginLeft,
                    yAdvance,
                    this->width - this->marginLeft - this->marginRight,
                    this->height - yAdvance - this->marginBottom
                );
            else
                child->view->setBoundaries(this->x + this->marginLeft,
                    yAdvance,
                    this->width - this->marginLeft - this->marginRight,
                    childHeight
                );

            child->view->layout();

            yAdvance += this->spacing + childHeight;
        }
    }
    // Horizontal orientation
    else if (this->orientation == BOXLAYOUT_HORIZONTAL)
    {
        unsigned xAdvance = this->x + this->marginLeft;
        for (BoxLayoutChild *child : this->children)
        {
            unsigned childWidth = child->view->getWidth();

            if (child->fill)
                child->view->setBoundaries(xAdvance,
                    this->y + this->marginTop,
                    this->width - xAdvance - this->marginRight,
                    this->height - this->marginTop - this->marginBottom
                );
            else
                child->view->setBoundaries(xAdvance,
                    this->y + this->marginTop,
                    childWidth,
                    this->height - this->marginTop - this->marginBottom
                );

            child->view->layout();

            xAdvance += this->spacing + childWidth;
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