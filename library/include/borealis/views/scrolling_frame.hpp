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

#pragma once

#include <borealis/core/animation.hpp>
#include <borealis/views/box.hpp>

namespace brls
{

enum class ScrollingBehavior
{
    // Inputs scroll the view like the scroll wheel on a web page, focus changes only when the next view to focus is fully on screen
    // To work properly, there must be at least one focusable view in the "top" area of the frame (there should not be the need to scroll to see it)
    NATURAL,

    // The focused view is always in the center, inputs always change focus and scroll immediately
    CENTERED,
};

// TODO: horizontal scrolling, add a scrolling direction attribute
// TODO: Scrollbar

// A vertical-only frame that can scroll if its content overflows.
// This frame can only contain one child view.
// The content view is detached from the rest of the tree
// so that its height can grow as much as possible.
class ScrollingFrame : public Box
{
  public:
    ScrollingFrame();

    void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) override;
    void onChildFocusGained(View* directChild, View* focusedView) override;
    void willAppear(bool resetState) override;
    void addView(View* view) override;
    void removeView(View* view) override;
    void onLayout() override;
    void setPadding(float top, float right, float bottom, float left) override;
    void setPaddingTop(float top) override;
    void setPaddingRight(float right) override;
    void setPaddingBottom(float bottom) override;
    void setPaddingLeft(float left) override;

    /**
     * Sets the content view of this scrolling box. There can only be one
     * content view per scrolling box at a time.
     */
    void setContentView(View* view);

    /**
     * Sets the scrolling behavior of this scrolling frame.
     * Default is NATURAL.
     */
    void setScrollingBehavior(ScrollingBehavior behavior);

    static View* create();

  private:
    View* contentView = nullptr;

    bool updateScrollingOnNextFrame = false;

    float middleY = 0; // y + height/2
    float bottomY = 0; // y + height

    Animatable scrollY = 0.0f; // from 0.0f to 1.0f, in % of content view height

    void prebakeScrolling();
    bool updateScrolling(bool animated);
    void startScrolling(bool animated, float newScroll);
    void scrollAnimationTick();

    float getScrollingAreaTopBoundary();
    float getScrollingAreaHeight();

    float getContentHeight();

    ScrollingBehavior behavior = ScrollingBehavior::NATURAL;
};

} // namespace brls
