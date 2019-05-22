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

#define HEADER_HEIGHT 88
#define FOOTER_HEIGHT 73

#define SEPARATOR_SPACING 30

#define TITLE_SIZE  30
// TODO: Don't center title, hardcode Y position
// TODO: Resize title (fine tune on Switch)
#define TITLE_START 130

// TODO: Add hints system

void SettingsFrame::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, FrameContext *ctx)
{
    // Text
    // Title
    nvgFillColor(vg, ctx->theme->textColor);
    nvgFontSize(vg, TITLE_SIZE);
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + TITLE_START, y + HEADER_HEIGHT / 2, this->title.c_str(), nullptr);

    // TODO: Footer

    // TODO: Icon

    // Separators
    nvgFillColor(vg, ctx->theme->separatorColor);

    // Header
    nvgBeginPath(vg);
    nvgRect(vg, x + SEPARATOR_SPACING, y + HEADER_HEIGHT - 1, width - SEPARATOR_SPACING * 2, 1);
    nvgFill(vg);

    // Footer
    nvgBeginPath(vg);
    nvgRect(vg, x + SEPARATOR_SPACING, y + height - FOOTER_HEIGHT, width - SEPARATOR_SPACING * 2, 1);
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

void SettingsFrame::layout()
{
    if (this->contentView)
    {
        this->contentView->setBoundaries(this->x, this->y + HEADER_HEIGHT, this->width, this->height - FOOTER_HEIGHT - HEADER_HEIGHT);
        this->contentView->layout();
    }
}

void SettingsFrame::setContentView(View *view)
{
    this->contentView = view;
    this->contentView->setParent(this);
    this->layout();
}

void SettingsFrame::setTitle(string title)
{
    this->title = title;
}

SettingsFrame::~SettingsFrame()
{

}