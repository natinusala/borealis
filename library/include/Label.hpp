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

#pragma once

#include <View.hpp>

// TODO: setText, which calls getParent()->invalidate()

enum class LabelStyle
{
    REGULAR = 0,
    SMALL,
    SUBLABEL,
    CRASH,
    BUTTON
};

// A Label, multiline or with a ticker
class Label : public View
{
    private:
        string text;

        bool multiline;
        unsigned fontSize;
        LabelStyle labelStyle;

        NVGalign horizontalAlign = NVG_ALIGN_LEFT;

    public:
        Label(LabelStyle labelStyle, string text, bool multiline = false);

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;
        void layout(NVGcontext* vg, Style *style, FontStash *stash) override;

        View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp = false) override
        {
            return nullptr;
        }

        void setHorizontalAlign(NVGalign align);
        void setText(string text);
};
