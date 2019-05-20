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

void SettingsFrame::frame(FrameContext *ctx)
{
    nvgSave(ctx->vg);
    nvgReset(ctx->vg);

    // Text
    // Title
    nvgFillColor(ctx->vg, ctx->theme->textColor);
    nvgFontSize(ctx->vg, TITLE_SIZE);
    nvgFontFaceId(ctx->vg, ctx->fontStash->regular);
    nvgTextAlign(ctx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(ctx->vg, this->x + TITLE_START, this->y + HEADER_HEIGHT / 2, this->title.c_str(), nullptr);

    // TODO: Footer

    // TODO: Icon

    // Separators
    nvgFillColor(ctx->vg, ctx->theme->separatorColor);

    // Header
    nvgBeginPath(ctx->vg);
    nvgRect(ctx->vg, this->x + SEPARATOR_SPACING, this->y + HEADER_HEIGHT - 1, this->width - SEPARATOR_SPACING * 2, 1);
    nvgFill(ctx->vg);

    // Footer
    nvgBeginPath(ctx->vg);
    nvgRect(ctx->vg, this->x + SEPARATOR_SPACING, this->y + this->height - FOOTER_HEIGHT, this->width - SEPARATOR_SPACING * 2, 1);
    nvgFill(ctx->vg);

    // Content view
    if (contentView)
        contentView->frame(ctx);

    nvgRestore(ctx->vg);
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
    this->layout();
}

void SettingsFrame::setTitle(string title)
{
    this->title = title;
}

SettingsFrame::~SettingsFrame()
{

}