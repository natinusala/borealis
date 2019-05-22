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

#include <View.hpp>

void View::frame(FrameContext *ctx)
{
    // Draw background
    this->drawBackground(ctx);

    // Draw the view
    this->draw(ctx);

    // Draw highlight
    // TODO: Fancy highlight
    if (this->focused)
    {
        nvgStrokeColor(ctx->vg, nvgRGB(255, 0, 0));
        nvgBeginPath(ctx->vg);
        nvgRect(ctx->vg, this->x - 5, this->y - 5, this->width + 10, this->height + 10);
        nvgStroke(ctx->vg);
    }
}

void View::setBackground(Background background)
{
    this->background = background;
}

static NVGcolor transparent = nvgRGBA(0, 0, 0, 0);

void View::drawBackground(FrameContext *ctx)
{
    switch (this->background)
    {
        case BACKGROUND_SIDEBAR:
        {
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
            break;
        }
        default:
            break;
    }
}

void View::setBoundaries(int x, int y, unsigned width, unsigned height)
{
    this->x         = x;
    this->y         = y;
    this->width     = width;
    this->height    = height;
}

void View::setParent(View *parent)
{
    this->parent = parent;
}

bool View::isFocused()
{
    return this->focused;
}

View* View::getParent()
{
    return this->parent;
}

void View::setWidth(unsigned width)
{
    this->width = width;
}

void View::setHeight(unsigned height)
{
    this->height = height;
}

int View::getX()
{
    return this->x;
}

int View::getY()
{
    return this->y;
}

unsigned View::getHeight()
{
    return this->height;
}

unsigned View::getWidth()
{
    return this->width;
}