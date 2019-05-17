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

void SettingsFrame::frame(FrameContext *ctx)
{
    nvgFontSize(ctx->vg, 36.0f);
    nvgFontFaceId(ctx->vg, ctx->fontStash->regular);
    nvgTextAlign(ctx->vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

    nvgFontBlur(ctx->vg, 0);
    nvgFillColor(ctx->vg, nvgRGB(255, 255, 255));
    nvgText(ctx->vg, this->width/2, this->height/2, "Hello World from SettingsFrame!", nullptr);
}

SettingsFrame::~SettingsFrame()
{

}