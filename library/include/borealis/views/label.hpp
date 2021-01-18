/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2021  natinusala
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

#pragma once

#include <borealis/core/animations.hpp>
#include <borealis/views/view.hpp>

namespace brls
{

enum class TextAlign
{
    LEFT,
    CENTER,
    RIGHT,
};

// Some text. The Label will automatically grow as much as possible.
// If there is enough space, the label dimensions will fit the text.
// If there is not enough horizontal space available, it will wrap and expand its height.
// If there is not enough vertical space available to wrap, the text will be truncated instead.
// The truncated text will animate if the Label or one of its parents is focused.
// Animation will be disabled if the alignment is other than LEFT.
// Warning: to wrap, the label width MUST be constrained
class Label : public View
{
  public:
    Label();
    ~Label();

    void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) override;
    void onLayout() override;
    void onFocusGained() override;
    void onFocusLost() override;
    void onParentFocusGained(View* focusedView) override;
    void onParentFocusLost(View* focusedView) override;

    /**
     * Sets the text of the label.
     */
    void setText(std::string text);

    /**
     * Sets the alignment of the text inside
     * the view. Will not move the view, only
     * the text inside.
     */
    void setTextAlign(TextAlign align);

    void setFontSize(float value);
    void setLineHeight(float value);
    void setTextColor(NVGcolor color);

    int getFont();
    float getFontSize();
    float getLineHeight();

    std::string getFullText();

    static View* create();

    void setRequiredWidth(float requiredWidth);
    void setEllipsisWidth(float ellipsisWidth);

    /**
     * If an "animated" label is too large to fit its bounds, the full text will
     * be displayed in a scrolling animation instead of being truncated
     * with an ellipsis ("...").
     *
     * Unless the autoAnimate flag is set to false, a label will automatically
     * be set to "animated" if it is focused or if one of its parent is.
     */
    void setAnimated(bool animated);

    /**
     * Controls whether focus changes automatically animates the label or not.
     * Doesn't stop the ongoing animation should there be one.
     */
    void setAutoAnimate(bool autoAnimate);

    /**
     * A single-line label will not wrap and increase its
     * height, even if the remaining vertical space allows it.
     *
     * Instead, it will always be truncated (if too large) to fit the horizontal space.
     */
    void setSingleLine(bool singleLine);

    bool isSingleLine();

    /**
     * Internal flag set by the measure function.
     */
    void setIsWrapping(bool isWrapping);

  private:
    std::string truncatedText = "";
    std::string fullText      = "";

    int font;
    float fontSize;
    float lineHeight;

    NVGcolor textColor;

    float requiredWidth;
    unsigned ellipsisWidth;

    bool singleLine = false;
    bool isWrapping = false;

    bool autoAnimate = true;
    bool animated    = false; // should it animate?
    bool animating   = false; // currently animating?

    menu_timer_t scrollingTimer;
    float scrollingAnimation;

    void stopScrollingAnimation();
    void resetScrollingAnimation();

    void startScrollTimer();
    void onScrollTimerFinished();

    TextAlign align = TextAlign::LEFT;

    enum NVGalign getNVGHorizontalAlign();
};

} // namespace brls
