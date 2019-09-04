/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam

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

#include <borealis/animations.hpp>
#include <borealis/frame_context.hpp>
#include <borealis/label.hpp>
#include <borealis/logger.hpp>
#include <borealis/style.hpp>
#include <borealis/task_manager.hpp>
#include <borealis/theme.hpp>
#include <borealis/view.hpp>
#include <vector>

namespace brls
{

class FramerateCounter : public Label
{
  private:
    retro_time_t lastSecond = 0;
    unsigned frames         = 0;

  public:
    FramerateCounter();

    void frame(FrameContext* ctx) override;
};

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
    static void pushView(View* view);

    /**
      * Pops the last pushed view from the stack
      * and gives focus back where it was before
      */
    static void popView();

    static void requestFocus(View* view, FocusDirection direction);

    static Style* getStyle();
    static void setTheme(Theme theme);
    static ThemeColors* getThemeColors();
    static ThemeColors* getThemeColorsForVariant(ThemeVariant variant);

    static void onGamepadButtonPressed(char button);

    /**
      * "Crashes" the app (displays a fullscreen CrashFrame)
      */
    static void crash(std::string text);

    static void quit();

    /**
      * Blocks any and all user inputs
      */
    static void blockInputs();

    /**
      * Unblocks inputs after a call to
      * blockInputs()
      */
    static void unblockInputs();

    static NVGcontext* getNVGContext();
    static TaskManager* getTaskManager();

    static void setCommonFooter(std::string footer);
    static std::string* getCommonFooter();

    static void setDisplayFramerate(bool enabled);
    static void toggleFramerateDisplay();

    static void setMaximumFPS(unsigned fps);

    inline static float windowScale; // public so that the glfw callback can access it

  private:
    inline static GLFWwindow* window;
    inline static NVGcontext* vg;

    inline static TaskManager* taskManager;

    inline static FontStash fontStash;

    inline static std::vector<View*> viewStack;
    inline static std::vector<View*> focusStack;

    inline static unsigned windowWidth, windowHeight;

    inline static View* currentFocus;

    inline static Theme currentTheme;
    inline static ThemeVariant currentThemeVariant;

    inline static GLFWgamepadstate oldGamepad;
    inline static GLFWgamepadstate gamepad;

    inline static Style* currentStyle;

    inline static unsigned blockInputsTokens = 0; // any value > 0 means inputs are blocked

    inline static std::string commonFooter = "";

    inline static FramerateCounter* framerateCounter = nullptr;

    static inline float frameTime = 0.0f;

    static void onWindowSizeChanged();

    static void setStyle(StyleEnum style);

    static void resizeFramerateCounter();

    static void frame();
    static void clear();
    static void exit();
};

} // namespace brls
