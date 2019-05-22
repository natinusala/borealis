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

#include <stdio.h>
#include <string>

using namespace std;

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

typedef enum
{
    BACKGROUND_NONE = 0,
    BACKGROUND_SIDEBAR
} Background;

class View
{
    private:
        Background background = BACKGROUND_NONE;

        void drawBackground(FrameContext *ctx);
    protected:
        unsigned x;
        unsigned y;
        unsigned width;
        unsigned height;

        bool focused = false;

        View *parent = nullptr;

    public:
        void setBoundaries(unsigned x, unsigned y, unsigned width, unsigned height);

        void setBackground(Background background);

        void setWidth(unsigned width);
        void setHeight(unsigned height);

        unsigned getX();
        unsigned getY();
        unsigned getWidth();
        unsigned getHeight();

        void setParent(View *parent);
        View* getParent();

        string name() const { return typeid(*this).name(); }

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

        bool isFocused();

        /**
         * Used to change focus based on controller
         * directions (up, down, left, right)
         *
         * Should return the view that is to be
         * focused or nullptr if no focusable
         * view exists in that direction
         * or if the view itself isn't focusable
         *
         * Should return this to give focus
         * to that one view, or traverse and
         * return a pointer to a child view
         *
         * This method should not be called
         * directly by the user, use
         * Application::requestFocus(View*) instead
         */
        virtual View *requestFocus(FocusDirection direction, bool fromUp = false)
        {
            if (this->parent)
                return this->parent->requestFocus(direction, true);

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