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

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <nanovg.h>

#include <vector>

#include <FrameContext.hpp>
#include <View.hpp>
#include <Theme.hpp>
#include <Style.hpp>

using namespace std;

class Application
{
    public:
        static bool init(StyleEnum style);
        static bool mainLoop();

        /**
         * Pushes a view on this applications's view stack
         *
         * The view will automatically be resized to take
         * the whole screen and layout() will be called
         *
         * The view will gain focus if applicable
         */
        static void pushView(View *view);

        static void requestFocus(View *view, FocusDirection direction);

        static Style* getStyle();

        static void onGamepadButtonPressed(char button);

    private:
        inline static GLFWwindow* window;
        inline static NVGcontext *vg;

        inline static FontStash fontStash;

        inline static vector<View*> viewStack;

        inline static unsigned windowWidth, windowHeight;

        inline static View* currentFocus;

        inline static Theme *currentTheme;

        inline static GLFWgamepadstate oldGamepad;
        inline static GLFWgamepadstate gamepad;

        inline static Style *currentStyle;

        static void onWindowSizeChanged();

        static void setStyle(StyleEnum style);

        static void frame();
        static void clear();
        static void exit();
};

