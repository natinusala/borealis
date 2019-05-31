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

Label::Label(LabelStyle labelStyle, string text, bool multiline) : text(text), multiline(multiline), labelStyle(labelStyle)
{
    Style *style = getStyle();
    switch (labelStyle)
    {
        case LABELSTYLE_REGULAR:
            this->fontSize = style->Label.regularFontSize;
            break;
        case LABELSTYLE_SUBLABEL:
            this->fontSize = style->Label.sublabelFontSize;
            break;
    }
}

void Label::layout(NVGcontext* vg, Style *style)
{
    // Update height if needed
    if (this->multiline)
    {
        float bounds[4];
        nvgFontSize(vg, this->fontSize);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgTextBoxBounds(vg, this->x, this->y, this->width, this->text.c_str(), nullptr, bounds);
        nvgTextLineHeight(vg, style->Label.lineHeight);
        this->height = bounds[3] - bounds[1]; // ymax - ymin
    }
}

void Label::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    switch (this->labelStyle)
    {
        case LABELSTYLE_REGULAR:
            nvgFillColor(vg, ctx->theme->textColor);
            break;
        case LABELSTYLE_SUBLABEL:
            nvgFillColor(vg, ctx->theme->sublabelColor);
            break;
    }

    nvgFontSize(vg, this->fontSize);

    if (this->multiline)
    {
        nvgTextLineHeight(vg, style->Label.lineHeight);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgTextBox(vg, x, y, width, this->text.c_str(), nullptr);
    }
    else
    {
        nvgTextLineHeight(vg, 1.0f);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_CENTER);
        nvgText(vg, x, y + height / 2, this->text.c_str(), nullptr); // TODO: Ticker
    }
}