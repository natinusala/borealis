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
#include <Animations.hpp>

#include <math.h>

void View::frame(FrameContext *ctx)
{
    // Draw background
    this->drawBackground(ctx->vg, ctx);

    // Draw the view
    this->draw(ctx->vg, this->x, this->y, this->width, this->height, ctx);

    // Draw highlight
    if (this->focused)
        this->drawHighlight(ctx->vg, ctx->theme);
}

#define HIGHLIGHT_STROKE_WIDTH      5
#define HIGHLIGHT_CORNER_RADIUS     0.5f

#define HIGHLIGHT_SHADOW_WIDTH      2
#define HIGHLIGHT_SHADOW_OFFSET     10
#define HIGHLIGHT_SHADOW_FEATHER    10
#define HIGHLIGHT_SHADOW_OPACITY    128

void View::drawHighlight(NVGcontext *vg, Theme *theme)
{
    unsigned inset  = this->getHighlightInset();
    unsigned x      = this->x - inset - HIGHLIGHT_STROKE_WIDTH/2;
    unsigned y      = this->y - inset - HIGHLIGHT_STROKE_WIDTH/2;
    unsigned width  = this->width   + inset * 2 + HIGHLIGHT_STROKE_WIDTH;
    unsigned height = this->height  + inset * 2 + HIGHLIGHT_STROKE_WIDTH;

    // Shadow
    NVGpaint shadowPaint = nvgBoxGradient(vg,
        x, y + HIGHLIGHT_SHADOW_WIDTH,
        width, height,
        HIGHLIGHT_CORNER_RADIUS*2, HIGHLIGHT_SHADOW_FEATHER,
        nvgRGBA(0, 0, 0, HIGHLIGHT_SHADOW_OPACITY), nvgRGBA(0, 0, 0, 0));

    nvgBeginPath(vg);
    nvgRect(vg, x - HIGHLIGHT_SHADOW_OFFSET, y - HIGHLIGHT_SHADOW_OFFSET,
        width + HIGHLIGHT_SHADOW_OFFSET*2, height + HIGHLIGHT_SHADOW_OFFSET*3);
    nvgRoundedRect(vg, x, y, width, height, HIGHLIGHT_CORNER_RADIUS);
    nvgPathWinding(vg, NVG_HOLE);
    nvgFillPaint(vg, shadowPaint);
    nvgFill(vg);

    // Border
    // TODO: Fix it on Switch
    float gradientX, gradientY, color;
    menu_animation_get_highlight(&gradientX, &gradientY, &color);

    NVGcolor pulsationColor = nvgRGBf((color * theme->highlightColor1.r) + (1-color) * theme->highlightColor2.r,
        (color * theme->highlightColor1.g) + (1-color) * theme->highlightColor2.g,
        (color * theme->highlightColor1.b) + (1-color) * theme->highlightColor2.b);

    NVGcolor borderColor = theme->highlightColor2;
    borderColor.a = 0.5f;

    NVGpaint border1Paint = nvgRadialGradient(vg,
        x + gradientX * width, y + gradientY * height,
        HIGHLIGHT_STROKE_WIDTH*10, HIGHLIGHT_STROKE_WIDTH*40,
        borderColor, nvgRGBA(0, 0, 0, 0));

    NVGpaint border2Paint = nvgRadialGradient(vg,
        x + (1-gradientX) * width, y + (1-gradientY) * height,
        HIGHLIGHT_STROKE_WIDTH*10, HIGHLIGHT_STROKE_WIDTH*40,
        borderColor, nvgRGBA(0, 0, 0, 0));

    nvgBeginPath(vg);
    nvgStrokeColor(vg, pulsationColor);
    nvgStrokeWidth(vg, HIGHLIGHT_STROKE_WIDTH);
    nvgRoundedRect(vg, x, y, width, height, HIGHLIGHT_CORNER_RADIUS);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgStrokePaint(vg, border1Paint);
    nvgStrokeWidth(vg, HIGHLIGHT_STROKE_WIDTH);
    nvgRoundedRect(vg, x, y, width, height, HIGHLIGHT_CORNER_RADIUS);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgStrokePaint(vg, border2Paint);
    nvgStrokeWidth(vg, HIGHLIGHT_STROKE_WIDTH);
    nvgRoundedRect(vg, x, y, width, height, HIGHLIGHT_CORNER_RADIUS);
    nvgStroke(vg);
}

void View::setBackground(Background background)
{
    this->background = background;
}

static NVGcolor transparent = nvgRGBA(0, 0, 0, 0);

#define BACKGROUND_SIDEBAR_BACKDROP_HEIGHT 16

void View::drawBackground(NVGcontext* vg, FrameContext *ctx)
{
    switch (this->background)
    {
        case BACKGROUND_SIDEBAR:
        {
            unsigned backdropHeight = BACKGROUND_SIDEBAR_BACKDROP_HEIGHT;

            // Solid color
            nvgBeginPath(vg);
            nvgFillColor(vg, ctx->theme->sidebarColor);
            nvgRect(vg, this->x, this->y + backdropHeight, this->width, this->height - backdropHeight * 2);
            nvgFill(vg);

            //Borders gradient
            // Top
            NVGpaint topGradient = nvgLinearGradient(vg, this->x, this->y + backdropHeight, this->x, this->y, ctx->theme->sidebarColor, transparent);
            nvgBeginPath(vg);
            nvgFillPaint(vg, topGradient);
            nvgRect(vg, this->x, this->y, this->width, backdropHeight);
            nvgFill(vg);

            // Bottom
            NVGpaint bottomGradient = nvgLinearGradient(vg, this->x, this->y + this->height - backdropHeight, this->x, this->y + this->height, ctx->theme->sidebarColor, transparent);
            nvgBeginPath(vg);
            nvgFillPaint(vg, bottomGradient);
            nvgRect(vg, this->x, this->y + this->height - backdropHeight, this->width, backdropHeight);
            nvgFill(vg);
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