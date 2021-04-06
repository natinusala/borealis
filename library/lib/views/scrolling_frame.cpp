/*
    Copyright 2020-2021 natinusala
    Copyright 2021 XITRIX

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <borealis/core/application.hpp>
#include <borealis/core/touch/scroll_gesture.hpp>
#include <borealis/core/touch/tap_gesture.hpp>
#include <borealis/core/util.hpp>
#include <borealis/views/scrolling_frame.hpp>

namespace brls
{

ScrollingFrame::ScrollingFrame()
{
    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "scrollingBehavior", ScrollingBehavior, this->setScrollingBehavior,
        {
            { "natural", ScrollingBehavior::NATURAL },
            { "centered", ScrollingBehavior::CENTERED },
        });

    this->setMaximumAllowedXMLElements(1);

    addGestureRecognizer(new ScrollGestureRecognizer([this](PanGestureStatus state) {
        if (state.state == GestureState::FAILED)
            return;

        float contentHeight = this->getContentHeight();

        if (state.deltaOnly)
        {
            float newScroll = (this->scrollY * contentHeight - (state.delta.y)) / contentHeight;
            startScrolling(false, newScroll);
            return;
        }

        static float startY;
        if (state.state == GestureState::START)
            startY = this->scrollY * contentHeight;

        float newScroll = (startY - (state.position.y - state.startPosition.y)) / contentHeight;

        // Start animation
        if (state.state != GestureState::END)
            startScrolling(false, newScroll);
        else
        {
            float time   = state.acceleration.time.y * 1000.0f;
            float newPos = this->scrollY * contentHeight + state.acceleration.distance.y;

            newScroll = newPos / contentHeight;

            if (newScroll == this->scrollY || time < 100)
                return;

            animateScrolling(newScroll, time);
        }
    },
        PanAxis::VERTICAL));

    // Stop scrolling on tap
    addGestureRecognizer(new TapGestureRecognizer([this](brls::TapGestureStatus status, Sound* soundToPlay) {
        if (status.state == GestureState::UNSURE)
            this->scrollY.stop();
    }));
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

    if (animated)
    {
        Style style = Application::getStyle();
        animateScrolling(newScroll, style["brls/animations/highlight"]);
    }
    else
    {
        this->scrollY.stop();
        this->scrollY = newScroll;
        this->scrollAnimationTick();
        this->invalidate();
    }
}

void ScrollingFrame::animateScrolling(float newScroll, float time)
{
    this->scrollY.stop();

    this->scrollY.reset();

    this->scrollY.addStep(newScroll, time, EasingFunction::quadraticOut);

    this->scrollY.setTickCallback([this] {
        this->scrollAnimationTick();
    });

    this->scrollY.start();

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
    {
        float contentHeight = this->getContentHeight();
        float bottomLimit   = (contentHeight - this->getScrollingAreaHeight()) / contentHeight;

        if (this->scrollY < 0)
            this->scrollY = 0;

        if (this->scrollY > bottomLimit)
            this->scrollY = bottomLimit;

        if (contentHeight <= getHeight())
            this->scrollY = 0;

        this->contentView->setTranslationY(-(this->scrollY * contentHeight));
    }
}

void ScrollingFrame::onChildFocusGained(View* directChild, View* focusedView)
{
    this->childFocused = true;

    // Start scrolling
    if (Application::getInputType() != InputType::TOUCH)
        this->updateScrolling(true);

    Box::onChildFocusGained(directChild, focusedView);
}

void ScrollingFrame::onChildFocusLost(View* directChild, View* focusedView)
{
    this->childFocused = false;
}

bool ScrollingFrame::updateScrolling(bool animated)
{
    if (!this->contentView || !this->childFocused)
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
