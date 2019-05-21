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

typedef enum
{
    FOCUSDIRECTION_NONE = 0,
    FOCUSDIRECTION_UP,
    FOCUSDIRECTION_DOWN,
    FOCUSDIRECTION_LEFT,
    FOCUSDIRECTION_RIGHT
} FocusDirection;

class View
{
    protected:
        unsigned x;
        unsigned y;
        unsigned width;
        unsigned height;

        bool focused = false;

    public:
        void setBoundaries(unsigned x, unsigned y, unsigned width, unsigned height);

        void setWidth(unsigned width);
        void setHeight(unsigned height);

        unsigned getX();
        unsigned getY();
        unsigned getWidth();
        unsigned getHeight();

        /**
         * Called each frame
         */
        void frame(FrameContext *ctx);

        /**
         * Called by frame() to draw
         * the view onscreen
         */
        virtual void draw(FrameContext *ctx) = 0;

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

        /**
         * Used to change focus based on controller
         * directions (up, down, left, right)
         *
         * Should return the view that is to be
         * focused or nullptr if no focusable
         * view exists in that direction
         *
         * Should return this to give focus
         * to that one view, or traverse and
         * return a pointer to a child view
         *
         * This method should not be called
         * directly by the user, use
         * Application::requestFocus(View*) instead
         */
        virtual View *requestFocus(FocusDirection direction)
        {
            return nullptr;
        }

        /**
         * Fired when focus is gained
         */
        virtual void onFocusGained()
        {
            this->focused = true;
        }

        /**
         * Fired when focus is lost
         */
        virtual void onFocusLost()
        {
            this->focused = false;
        }

        virtual ~View() {};
};