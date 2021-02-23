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
#include <borealis/core/util.hpp>
#include <borealis/views/scrolling_frame.hpp>
#include <borealis/core/touch/pan_gesture.hpp>

namespace brls
{

// TODO: don't forget absolute positioning
// TODO: RestoreFocus flag in Box to remember focus (sidebar...)
// TODO: use a menu timer in tabframe to defer the tab loading on input like HOS does
// TODO: fancy pants scrolling
// TODO: clarify addView / removeView -> setContentView, add it for everything or remove it
// TODO: streamline TransitionAnimation, it should be generic to activities
// TODO: does the fade in animation work?
// TODO: use fmt to format the ugly XML logic_errors
// TODO: find a way to reduce the number of invalidations on boot
// TODO: use HasNewLayout and MarkAsSeen around onLayout() (so in the event ? does it work ? or does the event only trigger on new layouts already ? or on all layouts ?)

// TODO: rework the highlight pulsation animation, it's not good enough

// TODO: it's time to do proper documentation using doxygen or whatever

// TODO: recycling, asynctask

// TODO: translate everything in fr
// TODO: make sure all useless invalidate calls are removed
// TODO: solve all TODOs in the diff
// TODO: clean the other TODOs

// TODO: ASAN time
// TODO: decomment everything in borealis.hpp
// TODO: cleanup the old example files
// TODO: copyright on all changed files
// TODO: clean the project board and rewrite documentation once this is out
// TODO: change the brls description once this is out: it's a cross-platform controller / gaming console oriented UI library with a Switch look and feel

ScrollingFrame::ScrollingFrame()
{
    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "scrollingBehavior", ScrollingBehavior, this->setScrollingBehavior,
        {
            { "natural", ScrollingBehavior::NATURAL },
            { "centered", ScrollingBehavior::CENTERED },
        });

    this->setMaximumAllowedXMLElements(1);

    addGestureRecognizer(new PanGestureRecognizer([this](PanGestureRecognizer* pan)
    {
        float contentHeight = this->getContentHeight();

        static float startY;
        if (pan->getState() == GestureState::START) 
            startY = this->scrollY * contentHeight;

        float newScroll = (startY - (pan->getY() - pan->getStartY())) / contentHeight;
        float bottomLimit = (contentHeight - this->getScrollingAreaHeight()) / contentHeight;
        
        // Bottom boundary
        if (newScroll > bottomLimit)
            newScroll = bottomLimit;

        // Top boundary
        if (newScroll < 0.0f)
            newScroll = 0.0f;

        // Start animation
        if (pan->getState() != GestureState::END)
            startScrolling(true, newScroll);
        else
        {
            float time = pan->getAcceleration().timeY * 1000.0f;
            float newPos = this->scrollY * contentHeight + pan->getAcceleration().distanceY;
            
            // Bottom boundary
            float bottomLimit = contentHeight - this->getScrollingAreaHeight();
            if (newPos > bottomLimit)
            {
                time = time * (1 - fabs(newPos - bottomLimit) / fabs(pan->getAcceleration().distanceY));
                newPos = bottomLimit;
            }
            
            // Top boundary
            if (newPos < 0)
            {
                time = time * (1 - fabs(newPos) / fabs(pan->getAcceleration().distanceY));
                newPos = 0;
            }
            
            newScroll = newPos / contentHeight;
            
            if (newScroll == this->scrollY || time < 100)
                return;

            animateScrolling(newScroll, time);
        }
    }, PanAxis::VERTICAL));
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
        this->contentView->setTranslationY(-(this->scrollY * this->getContentHeight()));
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
