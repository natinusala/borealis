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
#include <Animations.hpp>

#include <stdio.h>
#include <math.h>

BoxLayout::BoxLayout(BoxLayoutOrientation orientation) : orientation(orientation)
{

}

// TODO: Only draw children that are onscreen (use highlight rect + some margins)
void BoxLayout::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    // Enable scissoring
    nvgScissor(vg, x, y, this->width, this->height);

    // Draw children
    for (BoxLayoutChild *child : this->children)
        child->view->frame(ctx);

    //Disable scissoring
    nvgResetScissor(vg);
}

void BoxLayout::setSpacing(unsigned spacing)
{
    this->spacing = spacing;
    this->invalidate();
}

void BoxLayout::setMargins(unsigned top, unsigned right, unsigned bottom, unsigned left)
{
    this->marginBottom  = bottom;
    this->marginLeft    = left;
    this->marginRight   = right;
    this->marginTop     = top;
    this->invalidate();
}

size_t BoxLayout::getViewsCount()
{
    return this->children.size();
}

View* BoxLayout::defaultFocus(View *oldFocus)
{
    for (unsigned i = 0; i < this->children.size(); i++)
    {
        View *newFocus = this->children[i]->view->requestFocus(FocusDirection::NONE, oldFocus);
        if (newFocus)
        {
            this->focusedIndex = i;
            return newFocus;
        }
    }

    return nullptr;
}

View* BoxLayout::updateFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    // Give focus to first focusable view by default
    // or if we are going up in the tree and it's not
    // corresponding to our direction
    if (direction == FocusDirection::NONE)
    {
        View *newFocus = this->defaultFocus(oldFocus);
        if (newFocus)
            return newFocus;
    }
    // Handle directions
    else
    {
        // Give focus to next focusable view
        View *newFocus = nullptr;
        if ((this->orientation == BoxLayoutOrientation::HORIZONTAL && direction == FocusDirection::RIGHT) ||
            (this->orientation == BoxLayoutOrientation::VERTICAL && direction == FocusDirection::DOWN))
        {
            for (unsigned i = this->focusedIndex + 1; i < this->children.size(); i++)
            {
                newFocus = this->children[i]->view->requestFocus(FocusDirection::NONE, oldFocus);
                if (newFocus && newFocus != oldFocus)
                {
                    this->focusedIndex = i;
                    return newFocus;
                }
            }
            return nullptr;
        }
        else if ((this->orientation == BoxLayoutOrientation::HORIZONTAL && direction == FocusDirection::LEFT) ||
                    (this->orientation == BoxLayoutOrientation::VERTICAL && direction == FocusDirection::UP))
        {
            if (this->focusedIndex > 0)
            {
                for (unsigned i = this->focusedIndex - 1; i >= 0; i--)
                {
                    newFocus = this->children[i]->view->requestFocus(FocusDirection::NONE, oldFocus);
                    if (newFocus && newFocus != oldFocus)
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
    return View::requestFocus(direction, oldFocus);
}

void BoxLayout::setFocusedIndex(unsigned index)
{
    this->focusedIndex = index;
}

void BoxLayout::removeView(int index, bool free)
{
    BoxLayoutChild *toRemove = this->children[index];
    toRemove->view->willDisappear();
    if (free)
        delete toRemove->view;
    delete toRemove;
    this->children.erase(this->children.begin() + index);
}

void BoxLayout::updateScroll()
{
    // Don't scroll if layout hasn't been called yet
    if (this->entriesHeight == 0.0f)
        return;

    View *selectedView = this->children[this->focusedIndex]->view;
    int currentSelectionMiddleOnScreen = selectedView->getY() + selectedView->getHeight() / 2;
    float newScroll = this->scrollY - ((float)currentSelectionMiddleOnScreen - (float)this->middleY);

    // Bottom boundary
    if ((float)this->y + newScroll + (float)this->entriesHeight < (float)this->bottomY)
        newScroll = (float)this->height - (float)this->entriesHeight + (float)this->spacing - (float)this->marginTop - (float)this->marginBottom;

    // Top boundary
    if (newScroll > 0.0f)
        newScroll = 0.0f;

    if (newScroll == this->scrollY)
        return;

    //Start animation
    menu_animation_ctx_tag tag = (uintptr_t)&this->scrollY;

    menu_animation_kill_by_tag(&tag);

    menu_animation_ctx_entry_t entry;
    entry.cb            = nullptr;
    entry.duration      = VIEW_HIGHLIGHT_ANIMATION_DURATION;
    entry.easing_enum   = EASING_OUT_QUAD;
    entry.subject       = &this->scrollY;
    entry.tag           = tag;
    entry.target_value  = newScroll;
    entry.tick          = [this](void *userdata) { this->scrollAnimationTick(); };
    entry.userdata      = nullptr;

    menu_animation_push(&entry);

    this->invalidate();
}

View* BoxLayout::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    View *newFocus = this->updateFocus(direction, oldFocus,  fromUp);

    if (newFocus != nullptr)
        this->updateScroll();

    return newFocus;
}

void BoxLayout::scrollAnimationTick()
{
    this->invalidate();
}

void BoxLayout::prebakeScrolling()
{
    // Prebake values for scrolling
    this->middleY       = this->y + this->height/2;
    this->bottomY       = this->y + this->height;
    this->entriesHeight = 0.0f;
}

void BoxLayout::layout(NVGcontext* vg, Style *style)
{
    this->prebakeScrolling();

    // Vertical orientation
    if (this->orientation == BoxLayoutOrientation::VERTICAL)
    {
        int yAdvance = this->y + this->marginTop;
        for (size_t i = 0; i < this->children.size(); i++)
        {
            BoxLayoutChild *child = this->children[i];
            unsigned childHeight = child->view->getHeight();

            if (child->fill)
                child->view->setBoundaries(this->x + this->marginLeft,
                    yAdvance + roundf(this->scrollY),
                    this->width - this->marginLeft - this->marginRight,
                    this->height - yAdvance - this->marginBottom
                );
            else
                child->view->setBoundaries(this->x + this->marginLeft,
                    yAdvance + roundf(this->scrollY),
                    this->width - this->marginLeft - this->marginRight,
                    childHeight
                );

            child->view->layout(vg, style); // call layout directly in case height is updated
            childHeight = child->view->getHeight();

            int spacing = (int) this->spacing;
            View *next= i <= this->children.size()-2 ? this->children[i+1]->view : nullptr;

            this->customSpacing(child->view, next, &spacing);

            this->entriesHeight += spacing + childHeight;
            yAdvance            += spacing + childHeight;
        }
    }
    // Horizontal orientation
    // TODO: Make sure that spacing and margins work with horizontal
    else if (this->orientation == BoxLayoutOrientation::HORIZONTAL)
    {
        int xAdvance = this->x + this->marginLeft;
        for (size_t i = 0; i < this->children.size(); i++)
        {
            BoxLayoutChild *child = this->children[i];
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

            child->view->layout(vg, style); // call layout directly in case width is updated
            childWidth = child->view->getWidth();

            int spacing = (int) this->spacing;
            View *next= i <= this->children.size()-2 ? this->children[i+1]->view : nullptr;

            this->customSpacing(child->view, next, &spacing);

            xAdvance += spacing + childWidth;
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

    view->willAppear();
    this->invalidate();
}

bool BoxLayout::isEmpty()
{
    return this->children.size() == 0;
}

BoxLayout::~BoxLayout()
{
    for (BoxLayoutChild *child : this->children)
    {
        child->view->willDisappear();
        delete child->view;
        delete child;
    }
    this->children.clear();
}

void BoxLayout::willAppear()
{
    this->focusedIndex = 0;
    this->prebakeScrolling();
    for (BoxLayoutChild *child : this->children)
    {
        child->view->willAppear();
    }
}

void BoxLayout::willDisappear()
{
    for (BoxLayoutChild *child : this->children)
        child-> view->willDisappear();
}