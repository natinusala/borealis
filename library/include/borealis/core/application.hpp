/*
    Copyright 2019-2021 natinusala
    Copyright 2019 p-sam

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

#include <nanovg.h>
#include <tinyxml2.h>

#include <borealis/core/activity.hpp>
#include <borealis/core/audio.hpp>
#include <borealis/core/font.hpp>
#include <borealis/core/frame_context.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/core/platform.hpp>
#include <borealis/core/style.hpp>
#include <borealis/core/theme.hpp>
#include <borealis/core/view.hpp>
#include <borealis/views/label.hpp>
#include <unordered_map>
#include <vector>

namespace brls
{

typedef std::function<View*(void)> XMLViewCreator;

class Application
{
  public:
    /**
     * Inits the borealis application.
     * Returns true if it succeeded, false otherwise.
     */
    static bool init();

    /**
     * Creates the application window with the given title.
     * Must be called after calling init().
     */
    static void createWindow(std::string title);

    /**
     * Application main loop iteration.
     * Must be called in an infinite loop until it returns false.
     */
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

    /**
     * Loads a font from a given file and stores it in the font stash.
     * Returns true if the operation succeeded.
     */
    static bool loadFontFromFile(std::string fontName, std::string filePath);

    /**
     * Loads a font from a given memory buffer and stores it in the font stash.
     * Returns true if the operation succeeded.
     */
    static bool loadFontFromMemory(std::string fontName, void* data, size_t size, bool freeData);

    /**
     * Returns the nanovg handle to the given font name, or FONT_INVALID if
     * no such font is currently loaded.
     */
    static int getFont(std::string fontName);

    static void notify(std::string text);

    static void onControllerButtonPressed(enum ControllerButton button, bool repeating);

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

    static void setCommonFooter(std::string footer);
    static std::string* getCommonFooter();

    static void setDisplayFramerate(bool enabled);
    static void toggleFramerateDisplay();

    static void setMaximumFPS(unsigned fps);

    inline static float windowScale;

    /**
     * Sets whether BUTTON_START will globally be used to close the application.
     */
    static void setGlobalQuit(bool enabled);

    /**
     * Sets whether BUTTON_BACK will globally be used to toggle an FPS display.
     */
    static void setGlobalFPSToggle(bool enabled);

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

    /**
     * Returns the current system locale.
     */
    static std::string getLocale();

  private:
    inline static bool inited        = false;
    inline static bool quitRequested = false;

    inline static Platform* platform = nullptr;

    inline static std::string title;

    inline static FontStash fontStash;

    inline static std::vector<Activity*> activitiesStack;
    inline static std::vector<View*> focusStack;

    inline static unsigned windowWidth, windowHeight;

    inline static View* currentFocus;

    inline static unsigned blockInputsTokens = 0; // any value > 0 means inputs are blocked

    inline static std::string commonFooter = "";

    inline static bool globalQuitEnabled                     = false;
    inline static ActionIdentifier gloablQuitIdentifier      = ACTION_NONE;
    inline static bool globalFPSToggleEnabled                = false;
    inline static ActionIdentifier gloablFPSToggleIdentifier = ACTION_NONE;

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

    static ActionIdentifier registerFPSToggleAction(Activity* activity);
};

} // namespace brls
