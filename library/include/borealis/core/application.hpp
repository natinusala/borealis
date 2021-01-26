/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2021  natinusala
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

#include <nanovg/nanovg.h>
#include <tinyxml2.h>

#include <borealis/core/activity.hpp>
#include <borealis/core/animations.hpp>
#include <borealis/core/audio.hpp>
#include <borealis/core/frame_context.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/core/platform.hpp>
#include <borealis/core/style.hpp>
#include <borealis/core/task_manager.hpp>
#include <borealis/core/theme.hpp>
// #include <borealis/core/notification_manager.hpp> TODO: restore

#include <borealis/views/background.hpp>
#include <borealis/views/label.hpp>
#include <borealis/views/view.hpp>
// #include <borealis/views/hint.hpp> // TODO: restore

#include <unordered_map>
#include <vector>

namespace brls
{

// The top-right framerate counter
/*class FramerateCounter : public Label
{
  private:
    retro_time_t lastSecond = 0;
    unsigned frames         = 0;

  public:
    FramerateCounter();

    void frame(FrameContext* ctx) override;
}; TODO: restore that */

typedef std::function<View*(void)> XMLViewCreator;

class Application
{
  public:
    // Init with given style and theme, or use defaults if missing
    static bool init(std::string title);

    static bool mainLoop();

    static Platform* getPlatform();
    static AudioPlayer* getAudioPlayer();

    static NVGcontext* getNVGContext();
    inline static float contentWidth, contentHeight;

    /**
     * Called by the video context when the content window is resized
     * and when the context is ready (to setup the initial content scaling).
     */
    static void onWindowResized(int width, int height);

    // static void setBackground(Background* background);

    /**
      * Pushes a view on this applications's view stack.
      *
      * The view will automatically be resized to take
      * the whole screen.
      *
      * The view will gain focus if applicable.
      *
      * The first activity to be pushed cannot be popped.
      */
    static void pushActivity(Activity* view, TransitionAnimation animation = TransitionAnimation::FADE);

    /**
      * Pops the last pushed activity from the stack
      * and gives focus back where it was before.
      */
    static void popActivity(
        TransitionAnimation animation = TransitionAnimation::FADE, std::function<void(void)> cb = [] {});

    /**
     * Gives the focus to the given view
     * or clears the focus if given nullptr.
     */
    static void giveFocus(View* view);

    inline static Style getStyle()
    {
        return brls::getStyle();
    }

    static Theme getTheme();
    static ThemeVariant getThemeVariant();

    static int loadFont(const char* fontName, const char* filePath);
    static int loadFontFromMemory(const char* fontName, void* data, size_t size, bool freeData);
    static int findFont(const char* fontName);

    static FontStash* getFontStash();

    static void notify(std::string text);

    static void onGamepadButtonPressed(enum ControllerButton button, bool repeating);

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

    static TaskManager* getTaskManager();
    // static NotificationManager* getNotificationManager(); TODO: restore that

    static void setCommonFooter(std::string footer);
    static std::string* getCommonFooter();

    static void setDisplayFramerate(bool enabled);
    static void toggleFramerateDisplay();

    static void setMaximumFPS(unsigned fps);

    inline static float windowScale;

    static void resizeFramerateCounter();
    static void resizeNotificationManager();

    static GenericEvent* getGlobalFocusChangeEvent();
    static VoidEvent* getGlobalHintsUpdateEvent();

    static View* getCurrentFocus();

    static std::string getTitle();

    /**
     * Registers a view to be created from XML. You must give the name of the XML node as well
     * as a function that creates the view.
     *
     * If you need attributes, register them with the given functions in the view
     * class constructor directly. They will be called one by one after the view is instantiated.
     *
     * You should not add any children in the function, it is already taken care of.
     */
    static void registerXMLView(std::string name, XMLViewCreator creator);

    static bool XMLViewsRegisterContains(std::string name);
    static XMLViewCreator getXMLViewCreator(std::string name);

  private:
    inline static bool quitRequested = false;

    inline static Platform* platform = nullptr;

    inline static std::string title;

    inline static Background* background = nullptr;

    inline static TaskManager* taskManager;
    // inline static NotificationManager* notificationManager; TODO: restore that

    inline static FontStash fontStash;

    inline static std::vector<Activity*> activitiesStack;
    inline static std::vector<View*> focusStack; // TODO: move that to the activities stack

    inline static unsigned windowWidth, windowHeight;

    inline static View* currentFocus;

    inline static ThemeVariant currentThemeVariant;

    inline static ControllerState oldControllerState = {};
    inline static ControllerState controllerState    = {};

    inline static unsigned blockInputsTokens = 0; // any value > 0 means inputs are blocked

    inline static std::string commonFooter = "";

    // inline static FramerateCounter* framerateCounter = nullptr; TODO: restore that

    inline static float frameTime = 0.0f;

    inline static View* repetitionOldFocus = nullptr;

    inline static GenericEvent globalFocusChangeEvent;
    inline static VoidEvent globalHintsUpdateEvent;

    inline static std::unordered_map<std::string, XMLViewCreator> xmlViewsRegister;

    static void navigate(FocusDirection direction);

    static void onWindowSizeChanged();

    static void frame();
    static void clear();
    static void exit();

    /**
     * Handles actions for the currently focused view and
     * the given button
     * Returns true if at least one action has been fired
     */
    static bool handleAction(char button);

    static void registerBuiltInXMLViews();
};

} // namespace brls
