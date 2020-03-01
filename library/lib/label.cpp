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

#include <borealis/application.hpp>
#include <borealis/label.hpp>

namespace brls
{

Label::Label(LabelStyle labelStyle, std::string text, bool multiline)
    : text(text)
    , textTicker(text + "          " + text)
    , multiline(multiline)
    , labelStyle(labelStyle)
{
    Style* style     = Application::getStyle();
    this->lineHeight = style->Label.lineHeight;

    switch (labelStyle)
    {
        case LabelStyle::REGULAR:
            this->fontSize = style->Label.regularFontSize;
            break;
        case LabelStyle::MEDIUM:
            this->fontSize = style->Label.mediumFontSize;
            break;
        case LabelStyle::SMALL:
            this->fontSize = style->Label.smallFontSize;
            break;
        case LabelStyle::DESCRIPTION:
            this->fontSize = style->Label.descriptionFontSize;
            break;
        case LabelStyle::CRASH:
            this->fontSize = style->Label.crashFontSize;
            break;
        case LabelStyle::BUTTON_PLAIN_DISABLED:
        case LabelStyle::BUTTON_PLAIN:
        case LabelStyle::BUTTON_BORDERLESS:
        case LabelStyle::BUTTON_DIALOG:
            this->fontSize = style->Label.buttonFontSize;
            break;
        case LabelStyle::LIST_ITEM:
            this->fontSize = style->Label.listItemFontSize;
            break;
        case LabelStyle::NOTIFICATION:
            this->fontSize   = style->Label.notificationFontSize;
            this->lineHeight = style->Label.notificationLineHeight;
            break;
        case LabelStyle::DIALOG:
            this->fontSize = style->Label.dialogFontSize;
            break;
    }
}

Label::~Label() {
    menu_animation_ctx_tag tag = (uintptr_t) & this->tickerOffset;
    menu_animation_kill_by_tag(&tag);
}

void Label::setHorizontalAlign(NVGalign align)
{
    this->horizontalAlign = align;
}

void Label::setVerticalAlign(NVGalign align)
{
    this->verticalAlign = align;
}

void Label::setFontSize(unsigned size)
{
    this->fontSize = size;

    if (this->getParent())
        this->getParent()->invalidate();
}

void Label::setText(std::string text)
{
    this->text = text;

    if (this->hasParent())
        this->getParent()->invalidate();
}

void Label::setStyle(LabelStyle style)
{
    this->labelStyle = style;
}

void Label::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    nvgSave(vg);
    nvgReset(vg);

    nvgFontSize(vg, this->fontSize);
    nvgTextAlign(vg, this->horizontalAlign | NVG_ALIGN_TOP);
    nvgFontFaceId(vg, this->getFont(stash));
    nvgTextLineHeight(vg, this->lineHeight);

    float bounds[4];

    // Update width or height to text bounds
    if (this->multiline)
    {
        nvgTextBoxBounds(vg, this->x, this->y, this->width, this->text.c_str(), nullptr, bounds);

        this->height = bounds[3] - bounds[1]; // ymax - ymin
    }
    else
    {
        nvgTextBounds(vg, this->x, this->y, this->text.c_str(), nullptr, bounds);

        unsigned oldWidth = this->width;
        this->width = bounds[2] - bounds[0]; // xmax - xmin

        // offset the position to compensate the width change
        // and keep right alignment
        if (this->horizontalAlign == NVG_ALIGN_RIGHT)
            this->x += oldWidth - this->width;
    }

    nvgRestore(vg);
}

void Label::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    nvgFillColor(vg, this->getColor(ctx->theme));

    // Draw
    nvgFontSize(vg, this->fontSize);
    nvgFontFaceId(vg, this->getFont(ctx->fontStash));

    if (this->multiline)
    {
        nvgTextLineHeight(vg, this->lineHeight);
        nvgTextAlign(vg, this->horizontalAlign | NVG_ALIGN_TOP);
        nvgBeginPath(vg);

        nvgTextBox(vg, x, y, width, this->text.c_str(), nullptr);
    }
    else
    {
        nvgTextLineHeight(vg, 1.0f);
        nvgTextAlign(vg, this->horizontalAlign | this->verticalAlign);
        nvgBeginPath(vg);

        if (this->horizontalAlign == NVG_ALIGN_RIGHT)
            x += width;
        else if (this->horizontalAlign == NVG_ALIGN_CENTER)
            x += width / 2;

        if (this->textWidth == 0) {
            float bounds[4];
            nvgTextBounds(vg, x, y, this->text.c_str(), nullptr, bounds);
            this->textWidth = bounds[2] - bounds[0];
        }

        if (this->textTickerWidth == 0 && this->textWidth != 0) {
            float bounds[4];
            nvgTextBounds(vg, x, y, this->textTicker.c_str(), nullptr, bounds);
            this->textTickerWidth = (bounds[2] - bounds[0]) - this->textWidth;
            this->startTickerAnimation();
        }

        if (this->textWidth > this->getWidth())
        {   
            nvgSave(vg);
            nvgIntersectScissor(vg, x, y, width, height);

            if (this->verticalAlign == NVG_ALIGN_BOTTOM)
                nvgText(vg, x - tickerOffset, y + height, this->textTicker.c_str(), nullptr);
            else
                nvgText(vg, x - tickerOffset, y + height / 2, this->textTicker.c_str(), nullptr);   

            nvgRestore(vg);
        } 
        else 
        {
            if (this->verticalAlign == NVG_ALIGN_BOTTOM)
                nvgText(vg, x, y + height, this->text.c_str(), nullptr);
            else
                nvgText(vg, x, y + height / 2, this->text.c_str(), nullptr);
        }
    }
}

void Label::willAppear()
{

}

void Label::willDisappear()
{

}

void Label::startTickerAnimation()
{
    this->tickerWaitTimerCtx.duration = 1500;
    this->tickerWaitTimerCtx.cb = [&](void *userdata) {
        menu_animation_ctx_tag tag = (uintptr_t) & this->tickerOffset;
        menu_animation_kill_by_tag(&tag);

        this->tickerOffset = 0.0f;

        menu_animation_ctx_entry_t entry;
        entry.cb           = [&](void *userdata) { menu_timer_start(&this->tickerWaitTimer, &this->tickerWaitTimerCtx); };
        entry.duration     = this->textTickerWidth * 15;
        entry.easing_enum  = EASING_LINEAR;
        entry.subject      = &this->tickerOffset;
        entry.tag          = tag;
        entry.target_value = this->textTickerWidth;
        entry.tick         = [](void* userdata) {};
        entry.userdata     = nullptr;

        menu_animation_push(&entry);
    };

    menu_timer_start(&this->tickerWaitTimer, &this->tickerWaitTimerCtx);
}

void Label::stopTickerAnimation()
{
    this->tickerOffset = 0.0f;
    menu_timer_kill(&this->tickerWaitTimer);
}


void Label::setColor(NVGcolor color)
{
    this->customColor    = color;
    this->useCustomColor = true;
}

void Label::unsetColor()
{
    this->useCustomColor = false;
}

NVGcolor Label::getColor(ThemeValues* theme)
{
    // Use custom color if any
    if (this->useCustomColor)
        return a(this->customColor);

    switch (this->labelStyle)
    {
        case LabelStyle::DESCRIPTION:
            return a(theme->descriptionColor);
        case LabelStyle::CRASH:
            return RGB(255, 255, 255);
        case LabelStyle::BUTTON_PLAIN:
            return a(theme->buttonPlainEnabledTextColor);
        case LabelStyle::BUTTON_PLAIN_DISABLED:
            return a(theme->buttonPlainDisabledTextColor);
        case LabelStyle::NOTIFICATION:
            return a(theme->notificationTextColor);
        case LabelStyle::BUTTON_DIALOG:
            return a(theme->dialogButtonColor);
        default:
            return a(theme->textColor);
    }
}

void Label::setFont(int font)
{
    this->customFont    = font;
    this->useCustomFont = true;
}

void Label::unsetFont()
{
    this->useCustomFont = false;
}

int Label::getFont(FontStash* stash)
{
    if (this->useCustomFont)
        return this->customFont;

    return stash->regular;
}

} // namespace brls
