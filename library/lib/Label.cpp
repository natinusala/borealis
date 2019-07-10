/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

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

#include <Label.hpp>
#include <Application.hpp>

Label::Label(LabelStyle labelStyle, string text, bool multiline) : text(text), multiline(multiline), labelStyle(labelStyle)
{
    Style *style = Application::getStyle();
    switch (labelStyle)
    {
        case LabelStyle::REGULAR:
            this->fontSize = style->Label.regularFontSize;
            break;
        case LabelStyle::SMALL:
            this->fontSize = style->Label.smallFontSize;
            break;
        case LabelStyle::SUBLABEL:
            this->fontSize = style->Label.sublabelFontSize;
            break;
        case LabelStyle::CRASH:
            this->fontSize = style->Label.crashFontSize;
            break;
        case LabelStyle::BUTTON:
            this->fontSize = style->Label.buttonFontSize;
            break;
    }
}

void Label::setHorizontalAlign(NVGalign align)
{
    this->horizontalAlign = align;
}

void Label::setText(string text) {
    this->text = text;

    if (this->hasParent())
        this->getParent()->invalidate();
}

void Label::layout(NVGcontext* vg, Style *style, FontStash *stash)
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

void Label::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    switch (this->labelStyle)
    {
        case LabelStyle::REGULAR:
        case LabelStyle::SMALL:
        case LabelStyle::BUTTON:
            nvgFillColor(vg, a(ctx->theme->textColor));
            break;
        case LabelStyle::SUBLABEL:
            nvgFillColor(vg, a(ctx->theme->sublabelColor));
            break;
        case LabelStyle::CRASH:
            nvgFillColor(vg, RGB(255, 255, 255));
            break;
    }

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
        nvgTextAlign(vg, this->horizontalAlign | NVG_ALIGN_MIDDLE);
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgBeginPath(vg);
        nvgText(vg, x, y + height / 2, this->text.c_str(), nullptr); // TODO: Ticker
    }
}
