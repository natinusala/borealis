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
    DEBUG,
    BACKDROP
};

#define VIEW_HIGHLIGHT_ANIMATION_DURATION   100
#define VIEW_SHOW_ANIMATION_DURATION        250

extern NVGcolor transparent;

class View;

typedef function<void(View*)> EventListener;

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

        bool dirty = true;

        bool highlightShaking = false;
        retro_time_t highlightShakeStart;
        FocusDirection highlightShakeDirection;
        float highlightShakeAmplitude;

        bool fadeIn             = false; // is the fade in animation running?
        bool forceTranslucent   = false;

        Theme *themeOverride = nullptr;

        bool hidden = false;

    protected:
        int x = 0;
        int y = 0;

        unsigned width  = 0;
        unsigned height = 0;

        float collapseState = 1.0f;

        bool focused = false;

        View *parent = nullptr;

        EventListener focusListener = nullptr;

        virtual unsigned getShowAnimationDuration();

        virtual void getHighlightInsets(unsigned *top, unsigned *right, unsigned *bottom, unsigned *left)
        {
            *top    = 0;
            *right  = 0;
            *bottom = 0;
            *left   = 0;
        }

        virtual void getHighlightMetrics(Style *style, float *cornerRadius)
        {
            *cornerRadius = style->Highlight.cornerRadius;
        }

        virtual bool isHighlightBackgroundEnabled()
        {
            return true;
        }

        // Helper functions to apply this view's alpha to a color
        NVGcolor a(NVGcolor color);
        NVGpaint a(NVGpaint paint);

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

        /**
         * Draws the bottom-right buttons hint
         */
        void drawHint(FrameContext *ctx, unsigned x, unsigned y, unsigned width, unsigned height);

    public:
        void setBoundaries(int x, int y, unsigned width, unsigned height);

        void setBackground(Background background);

        void setWidth(unsigned width);
        void setHeight(unsigned height);

        void shakeHighlight(FocusDirection direction);

        int getX();
        int getY();
        unsigned getWidth();
        unsigned getHeight(bool includeCollapse = true);

        void setForceTranslucent(bool translucent);

        virtual void setParent(View *parent);
        View* getParent();
        bool hasParent();

        string name() const { return typeid(*this).name(); }

        /**
         * Called each frame
         * Do not override it to draw your view,
         * override draw() instead
         */
        virtual void frame(FrameContext *ctx);

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
        virtual void layout(NVGcontext* vg, Style *style, FontStash *stash)
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
         * Called when the show() animation (fade in)
         * ends
         */
        virtual void onShowAnimationEnd() { };

        /**
         * Shows the view (fade in animation)
         *
         * Called once when the view is
         * pushed to the view stack
         *
         * Not recursive
         */
        virtual void show(function<void(void)> cb, bool animate = true);

        /**
         * Hides the view in a collapse animation
         */
        void collapse(bool animated = true);

        bool isCollapsed();

        /**
         * Shows the view in a expand animation (opposite
         * of collapse)
         */
        void expand(bool animated = true);

        /**
         * Hides the view (fade out animation)
         *
         * Called if another view is pushed
         * on top of this one
         *
         * Not recursive
         */
        void hide(function<void(void)> cb, bool animated = true);

        bool isHidden();

        /**
         * Calls layout() on next frame
         */
        void invalidate()
        {
            this->dirty = true;
        }

        /**
         * Is this view translucent?
         *
         * If you override it please return
         * <value> || View::isTranslucent()
         * to keep the fadeIn transition
         */
        virtual bool isTranslucent()
        {
            return fadeIn || forceTranslucent;
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

        void setFocusListener(EventListener listener);

        float alpha = 1.0f;

        virtual float getAlpha(bool child = false);

        /**
         * Fires when the user "clicks" on the view
         * either via touch or pressing A
         *
         * Should return true if the event
         * was consumed, false otherwise
         */
        virtual bool onClick() { return false; };

        /**
         * Fires when the user presses "Back" on the view
         * either via touch (on hint) or pressing B
         *
         * Should return true if the event
         * was consumed, false otherwise
         */
        virtual bool onCancel() { return false; };

        // TODO: Play click animation here
        void click();
        void cancel();

        /**
         * Forces this view and its children to use
         * the specified theme
         */
        void overrideTheme(Theme *newTheme);

        virtual ~View();
};
