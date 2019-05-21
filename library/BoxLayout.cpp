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

void BoxLayout::draw(FrameContext *ctx)
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

View* BoxLayout::defaultFocus()
{
    for (unsigned i = 0; i < this->children.size(); i++)
    {
        View *newFocus = this->children[i]->view->requestFocus(FOCUSDIRECTION_NONE);
        if (newFocus)
        {
            this->focusedIndex = i;
            return newFocus;
        }
    }

    return nullptr;
}

// TODO: Allow sub classes to override default focus (active tab on Sidebar for instance)
View* BoxLayout::requestFocus(FocusDirection direction, bool fromUp)
{
    // Give focus to first focusable view by default
    // or if we are going up in the tree and it's not
    // corresponding to our direction
    if ((fromUp &&  (
                        (this->orientation == BOXLAYOUT_VERTICAL && (direction == FOCUSDIRECTION_RIGHT || direction == FOCUSDIRECTION_LEFT)) ||
                        (this->orientation == BOXLAYOUT_HORIZONTAL && (direction == FOCUSDIRECTION_DOWN || direction == FOCUSDIRECTION_UP))
                    )
        )
        || direction == FOCUSDIRECTION_NONE)
    {
        View *newFocus = this->defaultFocus();
        if (newFocus)
            return newFocus;
    }
    // Handle directions
    else
    {
        // Give focus to next focusable view
        View *newFocus = nullptr;
        if ((this->orientation == BOXLAYOUT_HORIZONTAL && direction == FOCUSDIRECTION_RIGHT) ||
            (this->orientation == BOXLAYOUT_VERTICAL && direction == FOCUSDIRECTION_DOWN))
        {
            for (unsigned i = this->focusedIndex + 1; i < this->children.size(); i++)
            {
                newFocus = this->children[i]->view->requestFocus(FOCUSDIRECTION_NONE);
                if (newFocus)
                {
                    this->focusedIndex = i;
                    return newFocus;
                }
            }
            return nullptr;
        }
        else if ((this->orientation == BOXLAYOUT_HORIZONTAL && direction == FOCUSDIRECTION_LEFT) ||
                    (this->orientation == BOXLAYOUT_VERTICAL && direction == FOCUSDIRECTION_UP))
        {
            if (this->focusedIndex > 0)
            {
                for (unsigned i = this->focusedIndex - 1; i >= 0; i--)
                {
                    newFocus = this->children[i]->view->requestFocus(FOCUSDIRECTION_NONE);
                    if (newFocus)
                    {
                        this->focusedIndex = i;
                        return newFocus;
                    }
                }
            }

            return nullptr;
        }
    }

    // Fallback to parent
    return View::requestFocus(direction);
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
    // TODO: Try spacing and margins
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

    view->setParent(this);

    this->children.push_back(child);
    this->layout();
}

bool BoxLayout::isEmpty()
{
    return this->children.size() == 0;
}

BoxLayout::~BoxLayout()
{
    for (BoxLayoutChild *child : this->children)
    {
        delete child->view;
        delete child;
    }
}