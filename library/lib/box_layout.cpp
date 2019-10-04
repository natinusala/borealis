/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  WerWolv
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

#include <math.h>
#include <stdio.h>

#include <borealis/animations.hpp>
#include <borealis/application.hpp>
#include <borealis/box_layout.hpp>
#include <borealis/logger.hpp>

namespace brls
{

BoxLayout::BoxLayout(BoxLayoutOrientation orientation, size_t defaultFocus)
    : orientation(orientation)
    , defaultFocusedIndex(defaultFocus)
    , focusedIndex(defaultFocus)
{
}

void BoxLayout::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    // Enable scissoring
    nvgSave(vg);
    nvgScissor(vg, x, y, this->width, this->height);

    // Draw children
    for (BoxLayoutChild* child : this->children)
        child->view->frame(ctx);

    //Disable scissoring
    nvgRestore(vg);
}

void BoxLayout::setSpacing(unsigned spacing)
{
    this->spacing = spacing;
    this->invalidate();
}

unsigned BoxLayout::getSpacing()
{
    return this->spacing;
}

void BoxLayout::setMargins(unsigned top, unsigned right, unsigned bottom, unsigned left)
{
    this->marginBottom = bottom;
    this->marginLeft   = left;
    this->marginRight  = right;
    this->marginTop    = top;
    this->invalidate();
}

void BoxLayout::setMarginBottom(unsigned bottom)
{
    this->marginBottom = bottom;
    this->invalidate();
}

size_t BoxLayout::getViewsCount()
{
    return this->children.size();
}

View* BoxLayout::defaultFocus(View* oldFocus)
{
    for (unsigned i = 0; i < this->children.size(); i++)
    {
        View* newFocus = this->children[i]->view->requestFocus(FocusDirection::NONE, oldFocus);
        if (newFocus)
        {
            this->focusedIndex = i;
            return newFocus;
        }
    }

    return nullptr;
}

View* BoxLayout::updateFocus(FocusDirection direction, View* oldFocus, bool fromUp)
{
    // Give focus to first focusable view by default
    if (direction == FocusDirection::NONE)
    {
        View* newFocus = this->defaultFocus(oldFocus);
        if (newFocus)
            return newFocus;
    }
    // Handle directions
    else
    {
        View* newFocus = nullptr;
        // Give focus to next focusable view
        if ((this->orientation == BoxLayoutOrientation::HORIZONTAL && direction == FocusDirection::RIGHT) || (this->orientation == BoxLayoutOrientation::VERTICAL && direction == FocusDirection::DOWN))
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
        // Give focus to previous focusable view
        else if ((this->orientation == BoxLayoutOrientation::HORIZONTAL && direction == FocusDirection::LEFT) || (this->orientation == BoxLayoutOrientation::VERTICAL && direction == FocusDirection::UP))
        {
            if (this->focusedIndex > 0)
            {
                for (int i = this->focusedIndex - 1; i >= 0; i--)
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
    BoxLayoutChild* toRemove = this->children[index];
    toRemove->view->willDisappear();
    if (free)
        delete toRemove->view;
    delete toRemove;
    this->children.erase(this->children.begin() + index);
}

void BoxLayout::clear(bool free)
{
    for (size_t i = 0; i < this->children.size(); i++)
        this->removeView(i, free);
}

void BoxLayout::updateScroll(bool animated)
{
    // Don't scroll if layout hasn't been called yet
    if (this->entriesHeight == 0.0f && this->orientation == BoxLayoutOrientation::VERTICAL)
        return;
    else if (this->entriesWidth == 0.0f && this->orientation == BoxLayoutOrientation::HORIZONTAL)
        return;

    View* selectedView = this->children[this->focusedIndex]->view;
    float newScroll;
    float* scroll;

    if (this->orientation == BoxLayoutOrientation::VERTICAL)
    {
        int currentSelectionMiddleOnScreen = selectedView->getY() + selectedView->getHeight() / 2;
        newScroll                          = this->scrollY - ((float)currentSelectionMiddleOnScreen - (float)this->middleY);
        scroll                             = &this->scrollY;

        if ((float)this->y + newScroll + (float)this->entriesHeight < (float)this->bottomY)
            newScroll = (float)this->height - (float)this->entriesHeight + (float)this->spacing - (float)this->marginTop - (float)this->marginBottom;
    }
    else if (this->orientation == BoxLayoutOrientation::HORIZONTAL)
    {
        if ((int)selectedView->getX() - (int)selectedView->getWidth() < (int)this->x) // If selection is out of view on the left
            newScroll = this->scrollX + ((float)this->x - (float)selectedView->getX()) + (float)selectedView->getWidth();
        else if (selectedView->getX() + selectedView->getWidth() * 2 > this->rightX) // If selection is out of view on right
            newScroll = this->scrollX + ((float)this->rightX - (float)selectedView->getX()) - (float)selectedView->getWidth() * 2;
        else
            return;

        scroll = &this->scrollX;
    }
    else
    {
        return;
    }

    // Top/Right boundary
    if (newScroll > 0.0f)
        newScroll = 0.0f;

    if (newScroll == *scroll)
        return;

    //Start animation
    menu_animation_ctx_tag tag = (uintptr_t)scroll;
    menu_animation_kill_by_tag(&tag);

    if (animated)
    {
        Style* style = Application::getStyle();

        menu_animation_ctx_entry_t entry;
        entry.cb           = [](void* userdata) {};
        entry.duration     = style->AnimationDuration.highlight;
        entry.easing_enum  = EASING_OUT_QUAD;
        entry.subject      = scroll;
        entry.tag          = tag;
        entry.target_value = newScroll;
        entry.tick         = [this](void* userdata) { this->scrollAnimationTick(); };
        entry.userdata     = nullptr;

        menu_animation_push(&entry);
    }
    else
    {
        *scroll = newScroll;
    }

    this->invalidate();
}

View* BoxLayout::requestFocus(FocusDirection direction, View* oldFocus, bool fromUp)
{
    View* newFocus = this->updateFocus(direction, oldFocus, fromUp);

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
    // Prebaked values for scrolling
    this->middleY       = this->y + this->height / 2;
    this->bottomY       = this->y + this->height + this->spacing;
    this->rightX        = this->x + this->width + this->spacing;
    this->entriesHeight = 0.0f;
    this->entriesWidth  = 0.0f;
}

void BoxLayout::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    this->prebakeScrolling();

    // Vertical orientation
    if (this->orientation == BoxLayoutOrientation::VERTICAL)
    {
        int yAdvance = this->y + this->marginTop;
        for (size_t i = 0; i < this->children.size(); i++)
        {
            BoxLayoutChild* child = this->children[i];
            unsigned childHeight  = child->view->getHeight();

            if (child->fill)
                child->view->setBoundaries(this->x + this->marginLeft,
                    yAdvance + roundf(this->scrollY),
                    this->width - this->marginLeft - this->marginRight,
                    this->y + this->height - yAdvance - this->marginBottom);
            else
                child->view->setBoundaries(this->x + this->marginLeft,
                    yAdvance + roundf(this->scrollY),
                    this->width - this->marginLeft - this->marginRight,
                    child->view->getHeight(false));

            child->view->layout(vg, style, stash); // call layout directly in case height is updated
            childHeight = child->view->getHeight();

            int spacing = (int)this->spacing;
            View* next  = (this->children.size() > 1 && i <= this->children.size() - 2) ? this->children[i + 1]->view : nullptr;

            this->customSpacing(child->view, next, &spacing);

            if (child->view->isCollapsed())
                spacing = 0;

            if (!child->view->isHidden())
                this->entriesHeight += spacing + childHeight;
            yAdvance += spacing + childHeight;
        }
    }
    // Horizontal orientation
    // TODO: Make sure that spacing and margins work with horizontal
    else if (this->orientation == BoxLayoutOrientation::HORIZONTAL)
    {
        int xAdvance = this->x + this->marginLeft;
        for (size_t i = 0; i < this->children.size(); i++)
        {
            BoxLayoutChild* child = this->children[i];
            unsigned childWidth   = child->view->getWidth();

            if (child->fill)
                child->view->setBoundaries(xAdvance + roundf(scrollX),
                    this->y + this->marginTop,
                    this->x + this->width - xAdvance - this->marginRight,
                    this->height - this->marginTop - this->marginBottom);
            else
                child->view->setBoundaries(xAdvance + roundf(scrollX),
                    this->y + this->marginTop,
                    childWidth,
                    this->height - this->marginTop - this->marginBottom);

            child->view->layout(vg, style, stash); // call layout directly in case width is updated
            childWidth = child->view->getWidth();

            int spacing = (int)this->spacing;

            View* next = i <= (this->children.size() > 1 && this->children.size() - 2) ? this->children[i + 1]->view : nullptr;

            this->customSpacing(child->view, next, &spacing);

            if (child->view->isCollapsed())
                spacing = 0;

            if (!child->view->isHidden())
                this->entriesWidth += spacing + childWidth;

            xAdvance += spacing + childWidth;
        }
    }

    // Setup initial scrolling
    if (this->firstLayout)
        this->updateScroll(false);

    this->firstLayout = false;
}

void BoxLayout::addView(View* view, bool fill)
{
    BoxLayoutChild* child = new BoxLayoutChild();
    child->view           = view;
    child->fill           = fill;

    view->setParent(this);

    this->children.push_back(child);

    view->willAppear();
    this->invalidate();
}

bool BoxLayout::isEmpty()
{
    return this->children.size() == 0;
}

bool BoxLayout::isChildFocused()
{
    for (auto child : this->children)
        if (child->view->isFocused())
            return true;

    return false;
}

BoxLayout::~BoxLayout()
{
    for (BoxLayoutChild* child : this->children)
    {
        child->view->willDisappear();
        delete child->view;
        delete child;
    }
    this->children.clear();
}

void BoxLayout::willAppear()
{
    this->focusedIndex = this->defaultFocusedIndex;
    this->prebakeScrolling();

    for (BoxLayoutChild* child : this->children)
        child->view->willAppear();
}

void BoxLayout::willDisappear()
{
    this->focusedIndex = this->defaultFocusedIndex;
    this->updateScroll(false);

    for (BoxLayoutChild* child : this->children)
        child->view->willDisappear();
}

} // namespace brls
