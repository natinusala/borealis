/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
    Copyright (C) 2019  p-sam
    Copyright (C) 2020  WerWolv

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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <borealis.hpp>
#include <chrono>
#include <set>
#include <string>
#include <thread>

// Constants used for scaling as well as
// creating a window of the right size on PC
constexpr uint32_t WINDOW_WIDTH  = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;

#define DEFAULT_FPS 60
#define BUTTON_REPEAT_DELAY 15
#define BUTTON_REPEAT_CADENCY 5

// glfw code from the glfw hybrid app by fincs
// https://github.com/fincs/hybrid_app

using namespace brls::i18n::literals;

namespace brls
{

bool Application::init(std::string title)
{
    return Application::init(title, Style::horizon(), Theme::horizon());
}

bool Application::init(std::string title, Style style, Theme theme)
{
    // Init rng
    std::srand(std::time(nullptr));

    // Init managers
    Application::taskManager         = new TaskManager();
    Application::notificationManager = new NotificationManager();

    // Init static variables
    Application::currentStyle = style;
    Application::currentFocus = nullptr;
    Application::title        = title;

    // Init theme to defaults
    Application::setTheme(theme);

    // Init nvg
    nvg::Initialize();

    // Load theme
#ifdef __SWITCH__
    ColorSetId nxTheme;
    setsysGetColorSetId(&nxTheme);

    if (nxTheme == ColorSetId_Dark)
        Application::currentThemeVariant = ThemeVariant_DARK;
    else
        Application::currentThemeVariant = ThemeVariant_LIGHT;
#else
    char* themeEnv = getenv("BOREALIS_THEME");
    if (themeEnv != nullptr && !strcasecmp(themeEnv, "DARK"))
        Application::currentThemeVariant = ThemeVariant_DARK;
    else
        Application::currentThemeVariant = ThemeVariant_LIGHT;
#endif

    // Init animations engine
    menu_animation_init();

    // Default FPS cap
    Application::setMaximumFPS(DEFAULT_FPS);

    return true;
}

bool Application::mainLoop()
{
    // Frame start
    retro_time_t frameStart = 0;
    if (Application::frameTime > 0.0f)
        frameStart = cpu_features_get_time_usec();

    if (!nvg::MainLoop())
    {
        nvg::Exit();
        return false;
    }

    // TODO
    Application::onGamepadButtonPressed(nvg::keys::Down(), false);

    // Animations
    menu_animation_update();

    // Tasks
    Application::taskManager->frame();

    // Render
    Application::frame();

    // Sleep if necessary
    if (Application::frameTime > 0.0f)
    {
        retro_time_t currentFrameTime = cpu_features_get_time_usec() - frameStart;
        retro_time_t frameTime        = (retro_time_t)(Application::frameTime * 1000);

        if (frameTime > currentFrameTime)
        {
            retro_time_t toSleep = frameTime - currentFrameTime;
            std::this_thread::sleep_for(std::chrono::microseconds(toSleep));
        }
    }

    return true;
}

void Application::quit()
{
    nvg::Break();
}

void Application::navigate(FocusDirection direction)
{
    View* currentFocus = Application::currentFocus;

    // Do nothing if there is no current focus or if it doesn't have a parent
    // (in which case there is nothing to traverse)
    if (!currentFocus || !currentFocus->hasParent())
        return;

    // Get next view to focus by traversing the views tree upwards
    View* nextFocus = currentFocus->getParent()->getNextFocus(direction, currentFocus->getParentUserData());

    while (!nextFocus) // stop when we find a view to focus
    {
        if (!currentFocus->hasParent() || !currentFocus->getParent()->hasParent()) // stop when we reach the root of the tree
            break;

        currentFocus = currentFocus->getParent();
        nextFocus    = currentFocus->getParent()->getNextFocus(direction, currentFocus->getParentUserData());
    }

    // No view to focus at the end of the traversal: wiggle and return
    if (!nextFocus)
    {
        Application::currentFocus->shakeHighlight(direction);
        return;
    }

    // Otherwise give it focus
    Application::giveFocus(nextFocus);
}

void Application::onGamepadButtonPressed(uint64_t button, bool repeating)
{
    if (Application::blockInputsTokens != 0)
        return;

    if (repeating && Application::repetitionOldFocus == Application::currentFocus)
        return;

    Application::repetitionOldFocus = Application::currentFocus;

    // Actions
    if (Application::handleAction(button))
        return;

    // Navigation
    // Only navigate if the button hasn't been consumed by an action
    // (allows overriding DPAD buttons using actions)
    if (button & KEY_DDOWN)
    {
        Application::navigate(FocusDirection::DOWN);
    }
    else if (button & KEY_DUP)
    {
        Application::navigate(FocusDirection::UP);
    }
    else if (button & KEY_DLEFT)
    {
        Application::navigate(FocusDirection::LEFT);
    }
    else if (button & KEY_DRIGHT)
    {
        Application::navigate(FocusDirection::RIGHT);
    }
}

View* Application::getCurrentFocus()
{
    return Application::currentFocus;
}

bool Application::handleAction(uint32_t button)
{
    if (Application::viewStack.empty())
        return false;

    View* hintParent = Application::currentFocus;
    std::set<Key> consumedKeys;

    if (!hintParent)
        hintParent = Application::viewStack[Application::viewStack.size() - 1];

    while (hintParent)
    {
        for (auto& action : hintParent->getActions())
        {
            if (action.key != static_cast<Key>(button))
                continue;

            if (consumedKeys.find(action.key) != consumedKeys.end())
                continue;

            if (action.available)
                if (action.actionListener())
                    consumedKeys.insert(action.key);
        }

        hintParent = hintParent->getParent();
    }

    return !consumedKeys.empty();
}

void Application::frame()
{
    nvg::Frame vg;

    // Frame context
    FrameContext frameContext = {
        .vg        = vg,
        .fontStash = &Application::fontStash,
        .theme     = Application::getThemeValues(),
    };

    // Clear background
    nvgBeginPath(vg);
    nvgFillColor(vg, frameContext.theme->backgroundColor);
    nvgRect(vg, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    nvgFill(vg);

    std::vector<View*> viewsToDraw;

    // Draw all views in the stack
    // until we find one that's not translucent
    for (size_t i = 0; i < Application::viewStack.size(); i++)
    {
        View* view = Application::viewStack[Application::viewStack.size() - 1 - i];
        viewsToDraw.push_back(view);

        if (!view->isTranslucent())
            break;
    }

    for (size_t i = 0; i < viewsToDraw.size(); i++)
    {
        View* view = viewsToDraw[viewsToDraw.size() - 1 - i];
        view->frame(&frameContext);
    }

    // Framerate counter
    if (Application::framerateCounter)
        Application::framerateCounter->frame(&frameContext);

    // Notifications
    Application::notificationManager->frame(&frameContext);
}

void Application::exit()
{
    Application::clear();

    nvg::Exit();

    menu_animation_free();

    if (Application::framerateCounter)
        delete Application::framerateCounter;

    delete Application::taskManager;
    delete Application::notificationManager;
}

void Application::setDisplayFramerate(bool enabled)
{
    if (!Application::framerateCounter && enabled)
    {
        Logger::debug("Enabling framerate counter");
        Application::framerateCounter = new FramerateCounter();
    }
    else if (Application::framerateCounter && !enabled)
    {
        Logger::debug("Disabling framerate counter");
        delete Application::framerateCounter;
        Application::framerateCounter = nullptr;
    }
}

void Application::toggleFramerateDisplay()
{
    Application::setDisplayFramerate(!Application::framerateCounter);
}

void Application::notify(std::string text)
{
    Application::notificationManager->notify(text);
}

NotificationManager* Application::getNotificationManager()
{
    return Application::notificationManager;
}

void Application::giveFocus(View* view)
{
    View* oldFocus = Application::currentFocus;
    View* newFocus = view ? view->getDefaultFocus() : nullptr;

    if (oldFocus != newFocus)
    {
        if (oldFocus)
            oldFocus->onFocusLost();

        Application::currentFocus = newFocus;
        Application::globalFocusChangeEvent.fire(newFocus);

        if (newFocus)
        {
            newFocus->onFocusGained();
            Logger::debug("Giving focus to {}", newFocus->describe());
        }
    }
}

void Application::popView(ViewAnimation animation, std::function<void(void)> cb)
{
    if (Application::viewStack.size() <= 1) // never pop the root view
        return;

    Application::blockInputs();

    View* last = Application::viewStack[Application::viewStack.size() - 1];
    last->willDisappear(true);

    last->setForceTranslucent(true);

    bool wait = animation == ViewAnimation::FADE; // wait for the new view animation to be done before showing the old one?

    // Hide animation (and show previous view, if any)
    last->hide([last, animation, wait, cb]() {
        last->setForceTranslucent(false);
        Application::viewStack.pop_back();
        delete last;

        // Animate the old view once the new one
        // has ended its animation
        if (Application::viewStack.size() > 0 && wait)
        {
            View* newLast = Application::viewStack[Application::viewStack.size() - 1];

            if (newLast->isHidden())
            {
                newLast->willAppear(false);
                newLast->show(cb, true, animation);
            }
            else
            {
                cb();
            }
        }

        Application::unblockInputs();
    },
        true, animation);

    // Animate the old view immediately
    if (!wait && Application::viewStack.size() > 1)
    {
        View* toShow = Application::viewStack[Application::viewStack.size() - 2];
        toShow->willAppear(false);
        toShow->show(cb, true, animation);
    }

    // Focus
    if (Application::focusStack.size() > 0)
    {
        View* newFocus = Application::focusStack[Application::focusStack.size() - 1];

        Logger::debug("Giving focus to {}, and removing it from the focus stack", newFocus->describe());

        Application::giveFocus(newFocus);
        Application::focusStack.pop_back();
    }
}

void Application::pushView(View* view, ViewAnimation animation)
{
    Application::blockInputs();

    // Call hide() on the previous view in the stack if no
    // views are translucent, then call show() once the animation ends
    View* last = nullptr;
    if (Application::viewStack.size() > 0)
        last = Application::viewStack[Application::viewStack.size() - 1];

    bool fadeOut = last && !last->isTranslucent() && !view->isTranslucent(); // play the fade out animation?
    bool wait    = animation == ViewAnimation::FADE; // wait for the old view animation to be done before showing the new one?

    view->registerAction("brls/hints/exit"_i18n, Key::PLUS, [] { Application::quit(); return true; });
    view->registerAction(
        "FPS", Key::MINUS, [] { Application::toggleFramerateDisplay(); return true; }, true);

    // Fade out animation
    if (fadeOut)
    {
        view->setForceTranslucent(true); // set the new view translucent until the fade out animation is done playing

        // Animate the new view directly
        if (!wait)
        {
            view->show([]() {
                Application::unblockInputs();
            },
                true, animation);
        }

        last->hide([animation, wait]() {
            View* newLast = Application::viewStack[Application::viewStack.size() - 1];
            newLast->setForceTranslucent(false);

            // Animate the new view once the old one
            // has ended its animation
            if (wait)
                newLast->show([]() { Application::unblockInputs(); }, true, animation);
        },
            true, animation);
    }

    view->setBoundaries(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    if (!fadeOut)
        view->show([]() { Application::unblockInputs(); }, true, animation);
    else
        view->alpha = 0.0f;

    // Focus
    if (Application::viewStack.size() > 0 && Application::currentFocus != nullptr)
    {
        Logger::debug("Pushing {} to the focus stack", Application::currentFocus->describe());
        Application::focusStack.push_back(Application::currentFocus);
    }

    // Layout and prepare view
    view->invalidate(true);
    view->willAppear(true);
    Application::giveFocus(view->getDefaultFocus());

    // And push it
    Application::viewStack.push_back(view);
}

void Application::clear()
{
    for (View* view : Application::viewStack)
    {
        view->willDisappear(true);
        delete view;
    }

    Application::viewStack.clear();
}

Style* Application::getStyle()
{
    return &Application::currentStyle;
}

void Application::setTheme(Theme theme)
{
    Application::currentTheme = theme;
}

ThemeValues* Application::getThemeValues()
{
    return &Application::currentTheme.colors[Application::currentThemeVariant];
}

ThemeValues* Application::getThemeValuesForVariant(ThemeVariant variant)
{
    return &Application::currentTheme.colors[variant];
}

ThemeVariant Application::getThemeVariant()
{
    return Application::currentThemeVariant;
}

/* Add those to the wrapper? */
int Application::loadFont(const char* fontName, const char* filePath)
{
    return nvgCreateFont(nvg::GetContext(), fontName, filePath);
}

int Application::loadFontFromMemory(const char* fontName, void* address, size_t size, bool freeData)
{
    return nvgCreateFontMem(nvg::GetContext(), fontName, (unsigned char*)address, size, freeData);
}

int Application::findFont(const char* fontName)
{
    return nvgFindFont(nvg::GetContext(), fontName);
}

void Application::crash(std::string text)
{
    CrashFrame* crashFrame = new CrashFrame(text);
    Application::pushView(crashFrame);
}

void Application::blockInputs()
{
    Application::blockInputsTokens += 1;
}

void Application::unblockInputs()
{
    if (Application::blockInputsTokens > 0)
        Application::blockInputsTokens -= 1;
}

NVGcontext* Application::getNVGContext()
{
    return nvg::GetContext();
}

TaskManager* Application::getTaskManager()
{
    return Application::taskManager;
}

void Application::setCommonFooter(std::string footer)
{
    Application::commonFooter = footer;
}

std::string* Application::getCommonFooter()
{
    return &Application::commonFooter;
}

FramerateCounter::FramerateCounter()
    : Label(LabelStyle::LIST_ITEM, "FPS: ---")
{
    this->setColor(nvgRGB(255, 255, 255));
    this->setVerticalAlign(NVG_ALIGN_MIDDLE);
    this->setHorizontalAlign(NVG_ALIGN_RIGHT);
    this->setBackground(ViewBackground::BACKDROP);

    Style* style                   = Application::getStyle();
    unsigned framerateCounterWidth = style->FramerateCounter.width;
    unsigned width                 = WINDOW_WIDTH;

    this->setBoundaries(
        width - framerateCounterWidth,
        0,
        framerateCounterWidth,
        style->FramerateCounter.height);

    this->lastSecond = cpu_features_get_time_usec() / 1000;
}

void FramerateCounter::frame(FrameContext* ctx)
{
    // Update counter
    retro_time_t current = cpu_features_get_time_usec() / 1000;

    if (current - this->lastSecond >= 1000)
    {
        char fps[10];
        snprintf(fps, sizeof(fps), "FPS: %03d", this->frames);
        this->setText(std::string(fps));
        this->invalidate(); // update width for background

        this->frames     = 0;
        this->lastSecond = current;
    }

    this->frames++;

    // Regular frame
    Label::frame(ctx);
}

void Application::setMaximumFPS(unsigned fps)
{
    if (fps == 0)
        Application::frameTime = 0.0f;
    else
    {
        Application::frameTime = 1000 / (float)fps;
    }

    Logger::info("Maximum FPS set to {} - using a frame time of {:.2f} ms", fps, Application::frameTime);
}

std::string Application::getTitle()
{
    return Application::title;
}

GenericEvent* Application::getGlobalFocusChangeEvent()
{
    return &Application::globalFocusChangeEvent;
}

VoidEvent* Application::getGlobalHintsUpdateEvent()
{
    return &Application::globalHintsUpdateEvent;
}

FontStash* Application::getFontStash()
{
    return &Application::fontStash;
}

} // namespace brls
