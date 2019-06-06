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
#include <Label.hpp>

enum class ButtonStyle
{
    PLAIN = 0,  // regular, plain button
    BORDERED,   // text and a border
    BORDERLESS, // only text
    CRASH       // same as borderless but with an increased corner radius
};

// A button
class Button : public View
{
    private:
        ButtonStyle style;
        Label *label;

        EventListener clickListener = nullptr;

    public:
        Button(ButtonStyle style, string label);
        ~Button();

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;
        bool onClick() override;
        void layout(NVGcontext* vg, Style *style, FontStash *stash);

        View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp) override
        {
            return this;
        }

        void getHighlightMetrics(Style *style, float *cornerRadius) override
        {
            *cornerRadius = this->style == ButtonStyle::CRASH ? style->Button.crashCornerRadius : style->Button.cornerRadius;
        }

        bool isHighlightBackgroundEnabled() override
        {
            return false;
        }

        void setClickListener(EventListener listener);
};