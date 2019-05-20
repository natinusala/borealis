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

#include <FrameContext.hpp>

// TODO: Make a lazy layout trigger: only reset layout if it's "dirty" at the beginning of frame() to avoid setting layout 654 times at startup
// also remove the calls to layout() when the view is added (never call layout() directly anymore)

// TODO: Separate frame in frame and draw - frame will setup nanovg stack, call draw, and draw highlight if necessary
// frame shouldn't be overridden by subclasses, draw will be (and is pure virtual)

class View
{
    protected:
        unsigned x;
        unsigned y;
        unsigned width;
        unsigned height;

    public:
        void setBoundaries(unsigned x, unsigned y, unsigned width, unsigned height);

        void setWidth(unsigned width);
        void setHeight(unsigned height);

        unsigned getX();
        unsigned getY();
        unsigned getWidth();
        unsigned getHeight();

        /**
         * Called each frame to
         * draw the view
         */
        virtual void frame(FrameContext *ctx) = 0;

        /**
         * Triggered when the view has been
         * resized and needs to layout its
         * children
         */
        virtual void layout() = 0;

        bool isTranslucent()
        {
            return false;
        }

        virtual ~View() {};
};