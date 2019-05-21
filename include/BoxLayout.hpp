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
    BOXLAYOUT_VERTICAL = 0,
    BOXLAYOUT_HORIZONTAL
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

        unsigned spacing = 0;

        unsigned marginTop      = 0;
        unsigned marginRight    = 0;
        unsigned marginBottom   = 0;
        unsigned marginLeft     = 0;
    public:
        BoxLayout(BoxLayoutOrientation orientation);

        void draw(FrameContext *ctx) override;
        void layout() override;

        /**
         * Sets spacing between views
         */
        void setSpacing(unsigned spacing);

        /**
         * Sets margins around views
         * Bottom (vertical) or right (horizontal) are
         * only effective if the last child is set to fill
         */
        void setMargins(unsigned top, unsigned right, unsigned bottom, unsigned left);

        /**
         * Adds a view to this box layout
         * If fill is set to true, the child will
         * fill the remaining space
         */
        void addView(View *view, bool fill = false);

        /**
         * Returns true if this layout
         * doesn't contain any views
         */
        bool isEmpty();

        ~BoxLayout();
};