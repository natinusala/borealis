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
#include <functional>

#include <features/features_cpu.h>

using namespace std;

enum class FocusDirection
{
    NONE = 0,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum class Background
{
    NONE = 0,
    SIDEBAR,
    DEBUG
};

#define VIEW_HIGHLIGHT_ANIMATION_DURATION 100

extern NVGcolor transparent;

// Superclass for all the other views
// Lifecycle of a view is :
//   new -> [willAppear -> willDisappear] -> delete
//
// Users have do to the new, the rest of the lifecycle is taken
// care of by the library
//
// willAppear and willDisappear can be called zero or multiple times
// before deletion (in case of a TabLayout for instance)
class View
{
    private:
        Background background = Background::NONE;

        void drawBackground(NVGcontext* vg, FrameContext *ctx, Style *style);
        void drawHighlight(NVGcontext *vg, Theme *theme, float alpha, Style *style, bool background);

        float highlightAlpha = 0.0f;

        bool dirty = false;

        bool highlightShaking = false;
        retro_time_t highlightShakeStart;
        FocusDirection highlightShakeDirection;
        float highlightShakeAmplitude;

    protected:
        int x;
        int y;
        unsigned width;

        unsigned height;

        bool focused = false;

        View *parent = nullptr;

        function<void(View*)> focusListener = nullptr;

        virtual void getHighlightInsets(unsigned *top, unsigned *right, unsigned *bottom, unsigned *left)
        {
            *top    = 0;
            *right  = 0;
            *bottom = 0;
            *left   = 0;
        }

        // Helper functions to apply this view's alpha to a color
        NVGcolor a(NVGcolor color);

        NVGcolor RGB(unsigned r, unsigned g, unsigned b)
        {
            return this->a(nvgRGB(r, g, b));
        }

        NVGcolor RGBA(unsigned r, unsigned g, unsigned b, unsigned a)
        {
            return this->a(nvgRGBA(r, g, b, a));
        }

        NVGcolor RGBf(float r, float g, float b)
        {
            return this->a(nvgRGBf(r, g, b));
        }

        NVGcolor RGBAf(float r, float g, float b, float a)
        {
            return this->a(nvgRGBAf(r, g, b, a));
        }

    public:
        void setBoundaries(int x, int y, unsigned width, unsigned height);

        void setBackground(Background background);

        void setWidth(unsigned width);
        void setHeight(unsigned height);

        void shakeHighlight(FocusDirection direction);

        int getX();
        int getY();
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
        virtual void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) = 0;

        /**
         * Triggered when the view has been
         * resized and needs to layout its
         * children
         */
        virtual void layout(NVGcontext* vg, Style *style)
        {
            // Nothing to do
        }

        /**
         * Called when the view will appear
         * on screen, before or after layout()
         *
         * Can be called if the view has
         * already appeared, so be careful
         */
        virtual void willAppear()
        {
            // Nothing to do
        }

        /**
         * Called when the view will disappear
         * from the screen
         *
         * Can be called if the view has
         * already disappeared, so be careful
         */
        virtual void willDisappear()
        {
            // Nothing to do
        }

        /**
         * Calls layout() on next frame
         */
        void invalidate()
        {
            this->dirty = true;
        }

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
        virtual View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp = false)
        {
            if (this->parent)
                return this->parent->requestFocus(direction, oldFocus, true);

            return nullptr;
        }

        /**
         * Fired when focus is gained
         */
        virtual void onFocusGained();

        /**
         * Fired when focus is lost
         */
        virtual void onFocusLost();

        void setFocusListener(function<void(View*)> listener);

        float alpha = 1.0f;

        float getAlpha();

        virtual ~View() {};
};