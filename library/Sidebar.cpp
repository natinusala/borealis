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

#include <Sidebar.hpp>

#define SIDEBAR_WIDTH 410

Sidebar::Sidebar() : BoxLayout(BOXLAYOUT_VERTICAL)
{
    this->setWidth(SIDEBAR_WIDTH);
}

static NVGcolor transparent = nvgRGBA(0, 0, 0, 0);

void Sidebar::frame(FrameContext *ctx)
{
    nvgSave(ctx->vg);
    nvgReset(ctx->vg);

    // Draw background
    unsigned backdropHeight = this->height / 16;

    // Solid color
    nvgBeginPath(ctx->vg);
    nvgFillColor(ctx->vg, ctx->theme->sidebarColor);
    nvgRect(ctx->vg, this->x, this->y + backdropHeight, this->width, this->height - backdropHeight * 2);
    nvgFill(ctx->vg);

    //Borders gradient
    // Top
    NVGpaint topGradient = nvgLinearGradient(ctx->vg, this->x, this->y + backdropHeight, this->x, this->y, ctx->theme->sidebarColor, transparent);
    nvgBeginPath(ctx->vg);
    nvgFillPaint(ctx->vg, topGradient);
    nvgRect(ctx->vg, this->x, this->y, this->width, backdropHeight);
    nvgFill(ctx->vg);

    // Bottom
    NVGpaint bottomGradient = nvgLinearGradient(ctx->vg, this->x, this->y + this->height - backdropHeight, this->x, this->y + this->height, ctx->theme->sidebarColor, transparent);
    nvgBeginPath(ctx->vg);
    nvgFillPaint(ctx->vg, bottomGradient);
    nvgRect(ctx->vg, this->x, this->y + this->height - backdropHeight, this->width, backdropHeight);
    nvgFill(ctx->vg);

    // Draw items
    BoxLayout::frame(ctx);

    nvgRestore(ctx->vg);
}