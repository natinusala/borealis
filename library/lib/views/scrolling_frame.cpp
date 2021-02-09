/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020-2021  natinusala

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

#include <borealis/core/animations.hpp>
#include <borealis/core/application.hpp>
#include <borealis/core/util.hpp>
#include <borealis/views/scrolling_frame.hpp>

namespace brls
{

// TODO: don't forget absolute positioning
// TODO: RestoreFocus flag in Box to remember focus (sidebar...)
// TODO: image (showcase + put in appletframe/tabframe header)
// TODO: use a menu timer in tabframe to defer the tab loading on input like HOS does
// TODO: button (border -> line) (showcase)
// TODO: fancy pants scrolling
// TODO: label alignment (showcase)
// TODO: clarify addView / removeView -> setContentView, add it for everything or remove it
// TODO: use the generic clickable stuff for everything clickable - add click pulse animation
// TODO: do the window thing to avoid doing glGetSize every frame or whatever
// TODO: streamline TransitionAnimation, it should be generic to activities
// TODO: does the fade in animation work?
// TODO: cleanup background, make it an XML attribute
// TODO: use fmt to format the ugly XML logic_errors
// TODO: find a way to reduce the number of invalidations on boot
// TODO: use HasNewLayout and MarkAsSeen around onLayout() (so in the event ? does it work ? or does the event only trigger on new layouts already ? or on all layouts ?)

// TODO: rework the highlight pulsation animation, it's not good enough

// TODO: it's time to do proper documentation using doxygen or whatever

// TODO: fix shitty frame pacer - try cpp high precision clock - see if it works fine on Switch, in which case only enable it there
// TODO: deko, recycling, audio, asynctask

// TODO: translate everything in fr
// TODO: make sure all useless invalidate calls are removed
// TODO: solve all TODOs in the diff
// TODO: clean the other TODOs

// TODO: ASAN time
// TODO: incomment everything in borealis.hpp
// TODO: cleanup the old example files
// TODO: copyright on all changed files
// TODO: clean the project board and rewrite documentation once this is out

ScrollingFrame::ScrollingFrame()
{
    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "scrollingBehavior", ScrollingBehavior, this->setScrollingBehavior,
        {
            { "natural", ScrollingBehavior::NATURAL },
            { "centered", ScrollingBehavior::CENTERED },
        });

    this->setMaximumAllowedXMLElements(1);
}

void ScrollingFrame::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    // Update scrolling - try until it works
    if (this->updateScrollingOnNextFrame && this->updateScrolling(false))
        this->updateScrollingOnNextFrame = false;

    // Enable scissoring
    nvgSave(vg);
    float scrollingTop    = this->getScrollingAreaTopBoundary();
    float scrollingHeight = this->getScrollingAreaHeight();
    nvgIntersectScissor(vg, x, scrollingTop, this->getWidth(), scrollingHeight);

    // Draw children
    Box::draw(vg, x, y, width, height, style, ctx);

    //Disable scissoring
    nvgRestore(vg);
}

void ScrollingFrame::addView(View* view)
{
    this->setContentView(view);
}

void ScrollingFrame::removeView(View* view)
{
    this->setContentView(nullptr);
}

void ScrollingFrame::setContentView(View* view)
{
    if (this->contentView)
    {
        Box::removeView(this->contentView); // will delete and call willDisappear
        this->contentView = nullptr;
    }

    if (!view)
        return;

    // Setup the view and add it
    this->contentView = view;

    view->detach();
    view->setCulled(false);
    view->setMaxWidth(this->getWidth());
    view->setDetachedPosition(this->getX(), this->getY());

    Box::addView(view); // will invalidate the scrolling box, hence calling onLayout and invalidating the contentView
}

void ScrollingFrame::onLayout()
{
    if (this->contentView)
    {
        this->contentView->setMaxWidth(this->getWidth());
        this->contentView->setDetachedPosition(this->getX(), this->getY());
        this->contentView->invalidate();
    }
}

float ScrollingFrame::getScrollingAreaTopBoundary()
{
    return this->getY();
}

float ScrollingFrame::getScrollingAreaHeight()
{
    return this->getHeight();
}

void ScrollingFrame::willAppear(bool resetState)
{
    this->prebakeScrolling();

    // First scroll all the way to the top
    // then wait for the first frame to scroll
    // to the selected view if needed (only known then)
    if (resetState)
    {
        this->startScrolling(false, 0.0f);
        this->updateScrollingOnNextFrame = true; // focus may have changed since
    }

    Box::willAppear(resetState);
}

void ScrollingFrame::prebakeScrolling()
{
    // Prebaked values for scrolling
    float y      = this->getScrollingAreaTopBoundary();
    float height = this->getScrollingAreaHeight();

    this->middleY = y + height / 2;
    this->bottomY = y + height;
}

void ScrollingFrame::startScrolling(bool animated, float newScroll)
{
    if (newScroll == this->scrollY)
        return;

    menu_animation_ctx_tag tag = (menu_animation_ctx_tag) & this->scrollY;
    menu_animation_kill_by_tag(&tag);

    if (animated)
    {
        Style style = Application::getStyle();

        menu_animation_ctx_entry_t entry;
        entry.cb           = [](void* userdata) {};
        entry.duration     = style["brls/animations/highlight"];
        entry.easing_enum  = EASING_OUT_QUAD;
        entry.subject      = &this->scrollY;
        entry.tag          = tag;
        entry.target_value = newScroll;
        entry.tick         = [this](void* userdata) { this->scrollAnimationTick(); };
        entry.userdata     = nullptr;

        menu_animation_push(&entry);
    }
    else
    {
        this->scrollY = newScroll;
    }

    this->invalidate();
}

void ScrollingFrame::setScrollingBehavior(ScrollingBehavior behavior)
{
    this->behavior = behavior;
}

float ScrollingFrame::getContentHeight()
{
    if (!this->contentView)
        return 0;

    return this->contentView->getHeight();
}

void ScrollingFrame::scrollAnimationTick()
{
    if (this->contentView)
        this->contentView->setTranslationY(-(this->scrollY * this->getContentHeight()));
}

void ScrollingFrame::onChildFocusGained(View* directChild, View* focusedView)
{
    // Start scrolling
    this->updateScrolling(true);

    Box::onChildFocusGained(directChild, focusedView);
}

bool ScrollingFrame::updateScrolling(bool animated)
{
    if (!this->contentView)
        return false;

    float contentHeight = this->getContentHeight();

    View* focusedView                  = Application::getCurrentFocus();
    int currentSelectionMiddleOnScreen = focusedView->getY() + focusedView->getHeight() / 2;
    float newScroll                    = -(this->scrollY * contentHeight) - (currentSelectionMiddleOnScreen - this->middleY);

    // Bottom boundary
    if (this->getScrollingAreaTopBoundary() + newScroll + contentHeight < this->bottomY)
        newScroll = this->getScrollingAreaHeight() - contentHeight;

    // Top boundary
    if (newScroll > 0.0f)
        newScroll = 0.0f;

    // Apply 0.0f -> 1.0f scale
    newScroll = abs(newScroll) / contentHeight;

    //Start animation
    this->startScrolling(animated, newScroll);

    return true;
}

#define NO_PADDING fatal("Padding is not supported by brls:ScrollingFrame, please set padding on the content view instead");

void ScrollingFrame::setPadding(float top, float right, float bottom, float left)
{
    NO_PADDING
}

void ScrollingFrame::setPaddingTop(float top)
{
    NO_PADDING
}

void ScrollingFrame::setPaddingRight(float right)
{
    NO_PADDING
}

void ScrollingFrame::setPaddingBottom(float bottom)
{
    NO_PADDING
}

void ScrollingFrame::setPaddingLeft(float left) {
    NO_PADDING
}

View* ScrollingFrame::create()
{
    return new ScrollingFrame();
}

} // namespace brls
