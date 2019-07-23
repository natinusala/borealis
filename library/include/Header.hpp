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

// A simple header with text, a rectangle on the left
// and a separator
class Header : public View
{
    private:
        string label;

    public:
        Header(string label);

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;

        View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp = false) override
        {
            return nullptr;
        }
};
