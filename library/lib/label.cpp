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
    , multiline(multiline)
    , labelStyle(labelStyle)
{
    Style* style = Application::getStyle();
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
            this->fontSize = style->Label.notificationFontSize;
            break;
        case LabelStyle::DIALOG:
            this->fontSize = style->Label.dialogFontSize;
            break;
    }
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
    // Update height if needed
    if (this->multiline)
    {
        nvgSave(vg);
        nvgReset(vg);

        float bounds[4];
        nvgFontSize(vg, this->fontSize);
        nvgTextAlign(vg, this->horizontalAlign | NVG_ALIGN_TOP);
        nvgFontFaceId(vg, stash->regular);
        nvgTextLineHeight(vg, style->Label.lineHeight);
        nvgTextBoxBounds(vg, this->x, this->y, this->width, this->text.c_str(), nullptr, bounds);

        this->height = bounds[3] - bounds[1]; // ymax - ymin

        nvgRestore(vg);
    }
}

void Label::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    // Use custom color if any
    if (this->useCustomColor)
        nvgFillColor(vg, a(this->customColor));
    else
    {
        switch (this->labelStyle)
        {
            case LabelStyle::REGULAR:
            case LabelStyle::MEDIUM:
            case LabelStyle::SMALL:
            case LabelStyle::LIST_ITEM:
            case LabelStyle::DIALOG:
            case LabelStyle::BUTTON_BORDERLESS:
                nvgFillColor(vg, a(ctx->theme->textColor));
                break;
            case LabelStyle::DESCRIPTION:
                nvgFillColor(vg, a(ctx->theme->descriptionColor));
                break;
            case LabelStyle::CRASH:
                nvgFillColor(vg, RGB(255, 255, 255));
                break;
            case LabelStyle::BUTTON_PLAIN:
                nvgFillColor(vg, a(ctx->theme->buttonPlainEnabledTextColor));
                break;
            case LabelStyle::BUTTON_PLAIN_DISABLED:
                nvgFillColor(vg, a(ctx->theme->buttonPlainDisabledTextColor));
                break;
            case LabelStyle::NOTIFICATION:
                nvgFillColor(vg, a(ctx->theme->notificationTextColor));
                break;
            case LabelStyle::BUTTON_DIALOG:
                nvgFillColor(vg, a(ctx->theme->dialogButtonColor));
                break;
        }
    }

    // Draw
    nvgFontSize(vg, this->fontSize);

    if (this->multiline)
    {
        nvgTextLineHeight(vg, style->Label.lineHeight);
        nvgTextAlign(vg, this->horizontalAlign | NVG_ALIGN_TOP);
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgBeginPath(vg);

        nvgTextBox(vg, x, y, width, this->text.c_str(), nullptr);
    }
    else
    {
        nvgTextLineHeight(vg, 1.0f);
        nvgTextAlign(vg, this->horizontalAlign | this->verticalAlign);
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgBeginPath(vg);

        if (this->horizontalAlign == NVG_ALIGN_RIGHT)
            x += width;
        else if (this->horizontalAlign == NVG_ALIGN_CENTER)
            x += width / 2;

        // TODO: Ticker

        if (this->verticalAlign == NVG_ALIGN_BOTTOM)
            nvgText(vg, x, y + height, this->text.c_str(), nullptr);
        else
            nvgText(vg, x, y + height / 2, this->text.c_str(), nullptr);
    }
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

} // namespace brls
