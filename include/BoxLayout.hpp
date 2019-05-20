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

#include <vector>

using namespace std;

// A basic horizontal or vertical box layout :
// - Children can currently only be stretched to full width (vertical) or height (horizontal)
// - Only works with children with fixed width (horizontal) or height (vertical)
// - Handles vertical scrolling only

typedef enum
{
    VERTICAL = 0,
    HORIZONTAL
} BoxLayoutOrientation;

class BoxLayoutChild
{
    public:
        View *view;
        bool fill; // should the child fill the remaining space?
};

// TODO: Allow disabling scrolling
// TODO: Add margins, pspacing
// TODO: Add alignment or stretching parameters to children
class BoxLayout : public View
{
    private:
        vector<BoxLayoutChild*> children;

        BoxLayoutOrientation orientation;
    public:
        BoxLayout(BoxLayoutOrientation orientation);

        void frame(FrameContext *ctx) override;
        void layout() override;

        /**
         * Adds a view to this box layout
         * If fill is set to true, the child will
         * fill the remaining space
         */
        void addView(View *view, bool fill = false);

        ~BoxLayout() { };
};