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

#include <SettingsFrame.hpp>
#include <nanovg.h>

// TODO: Don't center title, hardcode Y position
// TODO: Resize title (fine tune on Switch)

// TODO: Add hints system

void SettingsFrame::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    // Text
    // Title
    nvgFillColor(vg, ctx->theme->textColor);
    nvgFontSize(vg, style->settingsFrameTitleSize);
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + style->settingsFrameTitleStart, y + style->settingsFrameHeaderHeight / 2, this->title.c_str(), nullptr);

    // TODO: Footer

    // TODO: Icon

    // Separators
    nvgFillColor(vg, ctx->theme->separatorColor);

    // Header
    nvgBeginPath(vg);
    nvgRect(vg, x + style->settingsFrameSeparatorSpacing, y + style->settingsFrameHeaderHeight - 1, width - style->settingsFrameSeparatorSpacing * 2, 1);
    nvgFill(vg);

    // Footer
    nvgBeginPath(vg);
    nvgRect(vg, x + style->settingsFrameSeparatorSpacing, y + height - style->settingsFrameFooterHeight, width - style->settingsFrameSeparatorSpacing * 2, 1);
    nvgFill(vg);

    // Content view
    if (contentView)
        contentView->frame(ctx);
}

View* SettingsFrame::requestFocus(FocusDirection direction, bool fromUp)
{
    if (fromUp)
        return View::requestFocus(direction);
    else if (this->contentView)
        return this->contentView->requestFocus(direction);
    return nullptr;
}

void SettingsFrame::layout(Style *style)
{
    if (this->contentView)
    {
        this->contentView->setBoundaries(this->x, this->y + style->settingsFrameHeaderHeight, this->width, this->height - style->settingsFrameFooterHeight - style->settingsFrameHeaderHeight);
        this->contentView->layout(style);
    }
}

void SettingsFrame::setContentView(View *view)
{
    this->contentView = view;
    this->contentView->setParent(this);
    this->layout(getStyle());
}

void SettingsFrame::setTitle(string title)
{
    this->title = title;
}

SettingsFrame::~SettingsFrame()
{

}