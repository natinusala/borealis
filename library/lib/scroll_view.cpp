/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  natinusala

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

#include <borealis/scroll_view.hpp>
#include <borealis/application.hpp>

#include <math.h>

namespace brls
{

void ScrollView::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    if (!this->contentView)
        return;

    // Enable scissoring
    nvgSave(vg);
    nvgScissor(vg, x, y, this->width, this->height);

    // Draw content view
    this->contentView->frame(ctx);

    //Disable scissoring
    nvgRestore(vg);
}

unsigned ScrollView::getYCenter(View* view)
{
    return view->getY() + view->getHeight() / 2;
}

void ScrollView::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    // Layout content view
    if (this->contentView)
    {
        this->contentView->setBoundaries(
            this->getX(),
            this->getY() + roundf(this->scrollY),
            this->getWidth(),
            this->getHeight()
        );
        this->contentView->layout(vg, style, stash); // call layout directly to update height
    }

    // Compute yCenter
    if (!this->ready)
    {
        this->yCenter = this->getYCenter(this);
        this->ready = true;
    }

    // TODO: find a way to reset scrolling (without animating) without having an infinite layout -> scroll reset -> layout -> scroll reset... loop (cancel animation too!)
}

void ScrollView::willAppear()
{
    if (this->contentView)
        this->contentView->willAppear();

    // TODO: reset scrolling (cancel animation too!)
}

void ScrollView::willDisappear()
{
    if (this->contentView)
        this->contentView->willDisappear();
}

View* ScrollView::getDefaultFocus()
{
    return this->contentView;
}

void ScrollView::setContentView(View* view)
{
    this->contentView = view;

    if (this->contentView)
    {
        this->contentView->setParent(this);
        this->contentView->willAppear();
    }

    this->invalidate();
}

View* ScrollView::getContentView()
{
    return this->contentView;
}

void ScrollView::onChildFocusGained(View* child)
{
    // layout hasn't been called yet, don't scroll
    if (!this->ready)
        return;

    // Safety check to ensure that we don't have
    // two children (setContentView called twice)
    if (child != this->contentView)
        return;

    // Parameter child is the direct child of ours that gained focus,
    // so this->contentView in our case
    // We need to get the actually focused view instead
    View* focused = Application::getCurrentFocus();

    if (!focused)
        return;

    // Compute scroll
    unsigned focusedYCenter = this->getYCenter(focused);

    // TODO: animate (cancel animation too!)

    // TODO: get content view height and set scrollY accordingly

    // Boundaries check
    if (this->scrollY > 0.0f)
        this->scrollY = 0.0f;

    // TODO: bottom boundary check (needs accurate content view height)

    this->invalidate(); // TODO: remove once animated, if animation there is

    View::onChildFocusGained(child);
}

ScrollView::~ScrollView()
{
    if (this->contentView)
    {
         this->contentView->willDisappear();
         delete this->contentView;
    }
}

} // namespace brls
