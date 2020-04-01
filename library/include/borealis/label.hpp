/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
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

#include <borealis/view.hpp>
#include <borealis/animations.hpp>

namespace brls
{

enum class LabelStyle : std::uint32_t
{
    REGULAR = 0,
    MEDIUM,
    SMALL,
    DESCRIPTION,
    CRASH,
    BUTTON_PLAIN,
    BUTTON_PLAIN_DISABLED,
    BUTTON_BORDERLESS,
    LIST_ITEM,
    LIST_ITEM_VALUE,
    LIST_ITEM_VALUE_FAINT,
    NOTIFICATION,
    DIALOG,
    BUTTON_DIALOG
};

enum class LabelAnimation
{
    EASE_IN,
    EASE_OUT
};

// A Label, multiline or with a ticker
class Label : public View
{
  private:
    std::string text;
    std::string textTicker;
    std::string textEllipsis;

    unsigned textWidth = 0, textHeight = 0;
    unsigned textTickerWidth = 0;

    float tickerOffset = 0.0f;

    bool multiline;
    unsigned fontSize;
    float lineHeight;
    LabelStyle labelStyle, oldLabelStyle;

    NVGalign horizontalAlign = NVG_ALIGN_LEFT;
    NVGalign verticalAlign   = NVG_ALIGN_MIDDLE;

    NVGcolor customColor;
    bool useCustomColor = false;

    int customFont;
    bool useCustomFont = false;

    bool tickerActive = true;
    menu_timer_t tickerWaitTimer;
    menu_timer_ctx_entry_t tickerWaitTimerCtx;

    float textAnimation = 1.0f;

    GenericEvent::Subscription parentFocusSubscription;

    unsigned getFontSize(LabelStyle labelStyle);
    float getLineHeight(LabelStyle labelStyle);

    void onParentFocus();
    void onParentUnfocus();

  public:
    Label(LabelStyle labelStyle, std::string text, bool multiline = false);
    ~Label();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;

    View* requestFocus(FocusDirection direction, View* oldFocus, bool fromUp = false) override
    {
        return nullptr;
    }

    void resetValueAnimation();

    void setVerticalAlign(NVGalign align);
    void setHorizontalAlign(NVGalign align);
    void setText(std::string text);
    void setStyle(LabelStyle style);
    void setFontSize(unsigned size);

    void willAppear() override;
    void willDisappear() override;

    void startTickerAnimation();
    void stopTickerAnimation();

    unsigned getTextWidth();
    unsigned getTextHeight();
    const std::string& getText();

    /**
     * Sets the label color
     */
    void setColor(NVGcolor color);

    /**
     * Unsets the label color - it
     * will now use the default one
     * for the label style
     */
    void unsetColor();

    /**
     * Returns the effective label color
     * = custom or the style default
     */
    NVGcolor getColor(ThemeValues* theme);

    /**
     * Sets the font id
     */
    void setFont(int fontId);

    /**
     * Unsets the font id - it
     * will now use the regular one
     */
    void unsetFont();

    /**
     * Returns the font used
     * = custom or the regular font
     */
    int getFont(FontStash* stash);

    /**
     * Sets the ticker state to active (scrolling) or inactive (ellipsis)
     * 
     */
    void setTickerState(bool active);

    void animate(LabelAnimation animation);
};

} // namespace brls
