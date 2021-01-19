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
#include <stdexcept>
#include <string>

// The Switch include must always be BEFORE the Yoga Event include
// because libnx uses an "Event" struct that also exists in Yoga (name clash)
// and for some reason the compiler doesn't understand that Event in libnx
// does NOT mean facebook::yoga::Event (the namespace seems to be ignored)
#ifdef __SWITCH__
#include <switch.h>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define GLM_FORCE_PURE
#define GLM_ENABLE_EXPERIMENTAL
#include <nanovg/nanovg.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg/nanovg_gl.h>
#include <yoga/event/event.h>

#ifndef YG_ENABLE_EVENTS
#error Please enable Yoga events with the YG_ENABLE_EVENTS define
#endif

#include <chrono>
#include <set>
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

// TODO: Use this instead of a glViewport each frame
static void windowFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (!width || !height)
        return;

    glViewport(0, 0, width, height);
    Application::windowScale = (float)width / (float)WINDOW_WIDTH;

    float contentHeight = ((float)height / (Application::windowScale * (float)WINDOW_HEIGHT)) * (float)WINDOW_HEIGHT;

    Application::contentWidth  = WINDOW_WIDTH;
    Application::contentHeight = (unsigned)roundf(contentHeight);

    Application::resizeNotificationManager();

    Logger::info("Window size changed to {}x{}", width, height);
    Logger::info("New scale factor is {}", Application::windowScale);
}

static void joystickCallback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        Logger::info("Joystick {} connected", jid);
        if (glfwJoystickIsGamepad(jid))
            Logger::info("Joystick {} is gamepad: \"{}\"", jid, glfwGetGamepadName(jid));
    }
    else if (event == GLFW_DISCONNECTED)
        Logger::info("Joystick {} disconnected", jid);
}

static void errorCallback(int errorCode, const char* description)
{
    Logger::error("[GLFW:{}] {}", errorCode, description);
}

static void windowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        // Check for toggle-fullscreen combo
        if (key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT)
        {
            static int saved_x, saved_y, saved_width, saved_height;

            if (!glfwGetWindowMonitor(window))
            {
                // Back up window position/size
                glfwGetWindowPos(window, &saved_x, &saved_y);
                glfwGetWindowSize(window, &saved_width, &saved_height);

                // Switch to fullscreen mode
                GLFWmonitor* monitor    = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            }
            else
            {
                // Switch back to windowed mode
                glfwSetWindowMonitor(window, nullptr, saved_x, saved_y, saved_width, saved_height, GLFW_DONT_CARE);
            }
        }
    }
}

bool Application::init(std::string title)
{
    // Init platform
    Application::platform = Platform::createPlatform();

    if (!Application::platform)
        throw std::logic_error("Did not find a valid platform");

    Logger::info("Using platform {}", platform->getName());

    // Load most commonly used sounds
    AudioPlayer* audioPlayer = Application::getAudioPlayer();
    for (enum Sound sound : {
        SOUND_FOCUS_CHANGE,
        SOUND_FOCUS_ERROR,
        SOUND_CLICK,
    })
        audioPlayer->load(sound);

    // Init rng
    std::srand(std::time(nullptr));

    // Init managers
    Application::taskManager = new TaskManager();
    // Application::notificationManager = new NotificationManager(); TODO: restore

    // Init static variables
    Application::currentFocus = nullptr;
    Application::oldGamepad   = {};
    Application::gamepad      = {};
    Application::title        = title;

    // Init yoga
    YGConfig* defaultConfig       = YGConfigGetDefault();
    defaultConfig->useWebDefaults = true;

    yoga::Event::subscribe([](const YGNode& node, yoga::Event::Type eventType, yoga::Event::Data eventData) {
        View* view = (View*)node.getContext();

        if (!view)
            return;

        if (eventType == yoga::Event::NodeLayout)
            view->onLayout();
    });

    // Init glfw
    glfwSetErrorCallback(errorCallback);
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
    if (!glfwInit())
    {
        Logger::error("Failed to initialize glfw");
        return false;
    }

    // Create window
#ifdef __APPLE__
    // Explicitly ask for a 3.2 context on OS X
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Force scaling off to keep desired framebuffer size
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    Application::window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title.c_str(), nullptr, nullptr);
    if (!window)
    {
        Logger::error("glfw: failed to create window");
        glfwTerminate();
        return false;
    }

    // Configure window
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, windowFramebufferSizeCallback);
    glfwSetKeyCallback(window, windowKeyCallback);
    glfwSetJoystickCallback(joystickCallback);

    // Load OpenGL routines using glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    Logger::info("GL Vendor: {}", glGetString(GL_VENDOR));
    Logger::info("GL Renderer: {}", glGetString(GL_RENDERER));
    Logger::info("GL Version: {}", glGetString(GL_VERSION));

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        GLFWgamepadstate state;
        Logger::info("Gamepad detected: {}", glfwGetGamepadName(GLFW_JOYSTICK_1));
        glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
    }

    // Initialize the scene
    Application::vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    if (!vg)
    {
        Logger::error("Unable to init nanovg");
        glfwTerminate();
        return false;
    }

    windowFramebufferSizeCallback(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetTime(0.0);

    // Load fonts
#ifdef __SWITCH__
    {
        PlFontData font;

        // Standard font
        Result rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
        if (R_SUCCEEDED(rc))
        {
            Logger::info("Using Switch shared font");
            Application::fontStash.regular = Application::loadFontFromMemory("regular", font.address, font.size, false);
        }

        // Korean font
        rc = plGetSharedFontByType(&font, PlSharedFontType_KO);
        if (R_SUCCEEDED(rc))
        {
            Logger::info("Adding Switch shared Korean font");
            Application::fontStash.korean = Application::loadFontFromMemory("korean", font.address, font.size, false);
            nvgAddFallbackFontId(Application::vg, Application::fontStash.regular, Application::fontStash.korean);
        }

        // Extented font
        rc = plGetSharedFontByType(&font, PlSharedFontType_NintendoExt);
        if (R_SUCCEEDED(rc))
        {
            Logger::info("Using Switch shared symbols font");
            Application::fontStash.sharedSymbols = Application::loadFontFromMemory("symbols", font.address, font.size, false);
        }
    }
#else
    // Use illegal font if available
    if (access(BOREALIS_ASSET("Illegal-Font.ttf"), F_OK) != -1)
        Application::fontStash.regular = Application::loadFont("regular", BOREALIS_ASSET("Illegal-Font.ttf"));
    else
        Application::fontStash.regular = Application::loadFont("regular", BOREALIS_ASSET("inter/Inter-Switch.ttf"));

    if (Application::fontStash.regular == -1)
        brls::Logger::warning("Couldn't load regular font, no text will be displayed!");

    if (access(BOREALIS_ASSET("Wingdings.ttf"), F_OK) != -1)
        Application::fontStash.sharedSymbols = Application::loadFont("sharedSymbols", BOREALIS_ASSET("Wingdings.ttf"));
#endif

    // Material font
    if (access(BOREALIS_ASSET("material/MaterialIcons-Regular.ttf"), F_OK) != -1)
        Application::fontStash.material = Application::loadFont("material", BOREALIS_ASSET("material/MaterialIcons-Regular.ttf"));

    // Set symbols font as fallback
    if (Application::fontStash.sharedSymbols)
    {
        Logger::info("Using shared symbols font");
        nvgAddFallbackFontId(Application::vg, Application::fontStash.regular, Application::fontStash.sharedSymbols);
    }
    else
    {
        Logger::warning("Shared symbols font not found");
    }

    // Set Material as fallback
    if (Application::fontStash.material)
    {
        Logger::info("Using Material font");
        nvgAddFallbackFontId(Application::vg, Application::fontStash.regular, Application::fontStash.material);
    }
    else
    {
        Logger::warning("Material font not found");
    }

    // Load theme
#ifdef __SWITCH__
    ColorSetId nxTheme;
    setsysGetColorSetId(&nxTheme);

    if (nxTheme == ColorSetId_Dark)
        Application::currentThemeVariant = ThemeVariant::DARK;
    else
        Application::currentThemeVariant = ThemeVariant::LIGHT;
#else
    char* themeEnv = getenv("BOREALIS_THEME");
    if (themeEnv != nullptr && !strcasecmp(themeEnv, "DARK"))
        Application::currentThemeVariant = ThemeVariant::DARK;
    else
        Application::currentThemeVariant = ThemeVariant::LIGHT;
#endif

    // Init window size
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    Application::windowWidth  = viewport[2];
    Application::windowHeight = viewport[3];

    // Init animations engine
    menu_animation_init();

    // Default FPS cap
    Application::setMaximumFPS(DEFAULT_FPS);

    // Register built-in XML views
    Application::registerBuiltInXMLViews();

    return true;
}

bool Application::mainLoop()
{
    // Frame start
#ifdef __SWITCH__
    retro_time_t frameStart = 0;
    if (Application::frameTime > 0.0f)
        frameStart = cpu_features_get_time_usec();
#endif

    // glfw events
    bool isActive;
    do
    {
        isActive = !glfwGetWindowAttrib(Application::window, GLFW_ICONIFIED);
        if (isActive)
            glfwPollEvents();
        else
            glfwWaitEvents();

        if (glfwWindowShouldClose(Application::window))
        {
            Application::exit();
            return false;
        }
    } while (!isActive);

    // libnx applet main loop
#ifdef __SWITCH__
    if (!appletMainLoop())
    {
        Application::exit();
        return false;
    }
#endif

    // Gamepad
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &Application::gamepad))
    {
        // Keyboard -> DPAD Mapping
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]    = glfwGetKey(window, GLFW_KEY_LEFT);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]   = glfwGetKey(window, GLFW_KEY_RIGHT);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]      = glfwGetKey(window, GLFW_KEY_UP);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]    = glfwGetKey(window, GLFW_KEY_DOWN);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_START]        = glfwGetKey(window, GLFW_KEY_ESCAPE);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_BACK]         = glfwGetKey(window, GLFW_KEY_F1);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_A]            = glfwGetKey(window, GLFW_KEY_ENTER);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_B]            = glfwGetKey(window, GLFW_KEY_BACKSPACE);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]  = glfwGetKey(window, GLFW_KEY_L);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = glfwGetKey(window, GLFW_KEY_R);
    }

    // Trigger gamepad events
    // TODO: Translate axis events to dpad events here

    bool anyButtonPressed               = false;
    bool repeating                      = false;
    static retro_time_t buttonPressTime = 0;
    static int repeatingButtonTimer     = 0;

    for (int i = GLFW_GAMEPAD_BUTTON_A; i <= GLFW_GAMEPAD_BUTTON_LAST; i++)
    {
        if (Application::gamepad.buttons[i] == GLFW_PRESS)
        {
            anyButtonPressed = true;
            repeating        = (repeatingButtonTimer > BUTTON_REPEAT_DELAY && repeatingButtonTimer % BUTTON_REPEAT_CADENCY == 0);

            if (Application::oldGamepad.buttons[i] != GLFW_PRESS || repeating)
                Application::onGamepadButtonPressed(i, repeating);
        }

        if (Application::gamepad.buttons[i] != Application::oldGamepad.buttons[i])
            buttonPressTime = repeatingButtonTimer = 0;
    }

    if (anyButtonPressed && cpu_features_get_time_usec() - buttonPressTime > 1000)
    {
        buttonPressTime = cpu_features_get_time_usec();
        repeatingButtonTimer++; // Increased once every ~1ms
    }

    Application::oldGamepad = Application::gamepad;

    // Handle window size changes
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    unsigned newWidth  = viewport[2];
    unsigned newHeight = viewport[3];

    if (Application::windowWidth != newWidth || Application::windowHeight != newHeight)
    {
        Application::windowWidth  = newWidth;
        Application::windowHeight = newHeight;
        Application::onWindowSizeChanged();
    }

    // Animations
    menu_animation_update();

    // Tasks
    Application::taskManager->frame();

    // Render
    Application::frame();
    glfwSwapBuffers(window);

    // Sleep if necessary
#ifdef __SWITCH__
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
#endif

    return true;
}

Platform* Application::getPlatform()
{
    return Application::platform;
}

AudioPlayer* Application::getAudioPlayer()
{
    return Application::platform->getAudioPlayer();
}

void Application::quit()
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Application::navigate(FocusDirection direction)
{
    View* currentFocus = Application::currentFocus;

    // Do nothing if there is no current focus
    if (!currentFocus)
        return;

    View* nextFocus = nullptr;

    // Handle custom navigation routes
    // By View ptr
    if (currentFocus->hasCustomNavigationRouteByPtr(direction))
    {
        nextFocus = currentFocus->getCustomNavigationRoutePtr(direction);

        if (!nextFocus)
            Logger::warning("Tried to follow a navigation route that leads to a nullptr view! (from=\"" + currentFocus->describe() + "\", direction=" + std::to_string((int)direction) + ")");
    }
    // By ID
    else if (currentFocus->hasCustomNavigationRouteById(direction))
    {
        std::string id = currentFocus->getCustomNavigationRouteId(direction);
        nextFocus      = currentFocus->getNearestView(id);

        if (!nextFocus)
            Logger::warning("Tried to follow a navigation route that leads to an unknown view ID! (from=\"" + currentFocus->describe() + "\", direction=" + std::to_string((int)direction) + ", targetId=\"" + id + "\")");
    }
    // Do nothing if current focus doesn't have a parent
    // (in which case there is nothing to traverse)
    else if (currentFocus->hasParent())
    {
        // Get next view to focus by traversing the views tree upwards
        nextFocus = currentFocus->getParent()->getNextFocus(direction, currentFocus);

        while (!nextFocus) // stop when we find a view to focus
        {
            if (!currentFocus->hasParent() || !currentFocus->getParent()->hasParent()) // stop when we reach the root of the tree
                break;

            currentFocus = currentFocus->getParent();
            nextFocus    = currentFocus->getParent()->getNextFocus(direction, currentFocus);
        }
    }

    // No view to focus at the end of the traversal: wiggle and return
    if (!nextFocus)
    {
        Application::getAudioPlayer()->play(SOUND_FOCUS_ERROR);
        Application::currentFocus->shakeHighlight(direction);
        return;
    }

    // Otherwise give it focus
    enum Sound focusSound = nextFocus->getFocusSound();
    Application::getAudioPlayer()->play(focusSound);
    Application::giveFocus(nextFocus);
}

void Application::onGamepadButtonPressed(char button, bool repeating)
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
    switch (button)
    {
        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:
            Application::navigate(FocusDirection::DOWN);
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_UP:
            Application::navigate(FocusDirection::UP);
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:
            Application::navigate(FocusDirection::LEFT);
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:
            Application::navigate(FocusDirection::RIGHT);
            break;
        default:
            break;
    }
}

View* Application::getCurrentFocus()
{
    return Application::currentFocus;
}

bool Application::handleAction(char button)
{
    if (Application::activitiesStack.empty())
        return false;

    View* hintParent = Application::currentFocus;
    std::set<Key> consumedKeys;

    if (!hintParent)
        hintParent = Application::activitiesStack[Application::activitiesStack.size() - 1]->getContentView();

    while (hintParent)
    {
        for (auto& action : hintParent->getActions())
        {
            if (action.key != static_cast<Key>(button))
                continue;

            if (consumedKeys.find(action.key) != consumedKeys.end())
                continue;

            if (action.available)
            {
                if (action.actionListener(hintParent))
                {
                    Application::getAudioPlayer()->play(action.sound);
                    consumedKeys.insert(action.key);
                }
            }
        }

        hintParent = hintParent->getParent();
    }

    // Only play the error sound if action is a click
    if (consumedKeys.empty() && button == GLFW_GAMEPAD_BUTTON_A)
        Application::getAudioPlayer()->play(SOUND_CLICK_ERROR);

    return !consumedKeys.empty();
}

void Application::frame()
{
    // Frame context
    FrameContext frameContext = FrameContext();

    frameContext.pixelRatio = (float)Application::windowWidth / (float)Application::windowHeight;
    frameContext.vg         = Application::vg;
    frameContext.fontStash  = &Application::fontStash;
    frameContext.theme      = Application::getTheme();

    // GL Clear
    NVGcolor backgroundColor = frameContext.theme["brls/background"];
    glClearColor(
        backgroundColor.r,
        backgroundColor.g,
        backgroundColor.b,
        1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (Application::background)
        Application::background->preFrame();

    nvgBeginFrame(Application::vg, Application::windowWidth, Application::windowHeight, frameContext.pixelRatio);
    nvgScale(Application::vg, Application::windowScale, Application::windowScale);

    std::vector<View*> viewsToDraw;

    // Draw all activities in the stack
    // until we find one that's not translucent
    for (size_t i = 0; i < Application::activitiesStack.size(); i++)
    {
        Activity* activity = Application::activitiesStack[Application::activitiesStack.size() - 1 - i];

        View* view = activity->getContentView();
        if (view)
            viewsToDraw.push_back(view);

        if (!activity->isTranslucent())
            break;
    }

    if (Application::background)
        Application::background->frame(&frameContext);

    for (size_t i = 0; i < viewsToDraw.size(); i++)
    {
        View* view = viewsToDraw[viewsToDraw.size() - 1 - i];
        view->frame(&frameContext);
    }

    // Framerate counter
    /*if (Application::framerateCounter)
        Application::framerateCounter->frame(&frameContext); TODO: restore that */

    // Notifications
    // Application::notificationManager->frame(&frameContext); TODO: restore

    // End frame
    nvgResetTransform(Application::vg); // scale
    nvgEndFrame(Application::vg);

    if (Application::background)
        Application::background->postFrame();
}

void Application::exit()
{
    Application::clear();

    if (Application::vg)
        nvgDeleteGL3(Application::vg);

    glfwTerminate();

    menu_animation_free();

    /*if (Application::framerateCounter)
        delete Application::framerateCounter; TODO: restore that*/

    delete Application::taskManager;
    // delete Application::notificationManager; TODO: restore
    delete Application::platform;
}

void Application::setDisplayFramerate(bool enabled)
{
    /*if (!Application::framerateCounter && enabled)
    {
        Logger::debug("Enabling framerate counter");
        Application::framerateCounter = new FramerateCounter();
        Application::resizeFramerateCounter();
    }
    else if (Application::framerateCounter && !enabled)
    {
        Logger::debug("Disabling framerate counter");
        delete Application::framerateCounter;
        Application::framerateCounter = nullptr;
    } TODO: restore that */
}

void Application::toggleFramerateDisplay()
{
    // Application::setDisplayFramerate(!Application::framerateCounter); TODO: restore that
}

void Application::resizeFramerateCounter()
{
    //     if (!Application::framerateCounter)
    //         return;

    //     Style* style                   = Application::getStyle();
    //     unsigned framerateCounterWidth = style->FramerateCounter.width;
    //     unsigned width                 = WINDOW_WIDTH;

    //     Application::framerateCounter->setBoundaries(
    //         width - framerateCounterWidth,
    //         0,
    //         framerateCounterWidth,
    //         style->FramerateCounter.height);
    //     Application::framerateCounter->invalidate(); TODO: restore that
}

void Application::resizeNotificationManager()
{
    //     Application::notificationManager->setBoundaries(0, 0, Application::contentWidth, Application::contentHeight);
    //     Application::notificationManager->invalidate(); TODO: restore that
}

void Application::notify(std::string text)
{
    //Application::notificationManager->notify(text); TODO: restore
}

/* NotificationManager* Application::getNotificationManager()
{
    return Application::notificationManager;
} TODO: restore */

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

void Application::popActivity(TransitionAnimation animation, std::function<void(void)> cb)
{
    if (Application::activitiesStack.size() <= 1) // never pop the first activity
        return;

    Application::blockInputs();

    Activity* last = Application::activitiesStack[Application::activitiesStack.size() - 1];
    last->willDisappear(true);

    last->setInFadeAnimation(true);

    bool wait = animation == TransitionAnimation::FADE; // wait for the new activity animation to be done before showing the old one?

    // Hide animation (and show previous activity, if any)
    last->hide([last, animation, wait, cb] {
        last->setInFadeAnimation(false);
        Application::activitiesStack.pop_back();
        delete last;

        // Animate the old activity once the new one
        // has ended its animation
        if (Application::activitiesStack.size() > 0 && wait)
        {
            Activity* newLast = Application::activitiesStack[Application::activitiesStack.size() - 1];

            if (newLast->isHidden())
            {
                newLast->willAppear(false);
                newLast->show(cb, true, newLast->getShowAnimationDuration(animation));
            }
            else
            {
                cb();
            }
        }

        Application::unblockInputs();
    },
        true, last->getShowAnimationDuration(animation));

    // Animate the old activity immediately
    if (!wait && Application::activitiesStack.size() > 1)
    {
        Activity* toShow = Application::activitiesStack[Application::activitiesStack.size() - 2];
        toShow->willAppear(false);
        toShow->show(cb, true, toShow->getShowAnimationDuration(animation));
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

// TODO: transition animation should be an attribute of the activity, not given when pushing
void Application::pushActivity(Activity* activity, TransitionAnimation animation)
{
    Application::blockInputs();

    // Create the activity content view
    activity->setContentView(activity->createContentView());
    activity->onContentAvailable();

    // Call hide() on the previous activity in the stack if no
    // activities are translucent, then call show() once the animation ends
    Activity* last = nullptr;
    if (Application::activitiesStack.size() > 0)
        last = Application::activitiesStack[Application::activitiesStack.size() - 1];

    bool fadeOut = last && !last->isTranslucent() && !activity->isTranslucent(); // play the fade out animation?
    bool wait    = animation == TransitionAnimation::FADE; // wait for the old activity animation to be done before showing the new one?

    activity->registerAction("brls/hints/exit"_i18n, Key::PLUS, [](View* view) { Application::quit(); return true; });
    activity->registerAction(
        "FPS", Key::MINUS, [](View* view) { Application::toggleFramerateDisplay(); return true; }, true);

    // Fade out animation
    if (fadeOut)
    {
        activity->setInFadeAnimation(true); // set the new activity translucent until the fade out animation is done playing

        // Animate the new activity directly
        if (!wait)
        {
            activity->show([] {
                Application::unblockInputs();
            },
                true, activity->getShowAnimationDuration(animation));
        }

        last->hide([animation, wait] {
            Activity* newLast = Application::activitiesStack[Application::activitiesStack.size() - 1];
            newLast->setInFadeAnimation(false);

            // Animate the new activity once the old one
            // has ended its animation
            if (wait)
                newLast->show([] { Application::unblockInputs(); }, true, newLast->getShowAnimationDuration(animation));
        },
            true, last->getShowAnimationDuration(animation));
    }

    activity->resizeToFitWindow();

    if (!fadeOut)
        activity->show([] { Application::unblockInputs(); }, true, activity->getShowAnimationDuration(animation));
    else
        activity->setAlpha(0.0f);

    // Focus
    if (Application::activitiesStack.size() > 0 && Application::currentFocus != nullptr)
    {
        Logger::debug("Pushing {} to the focus stack", Application::currentFocus->describe());
        Application::focusStack.push_back(Application::currentFocus);
    }

    // Layout and prepare activity
    activity->willAppear(true);
    Application::giveFocus(activity->getDefaultFocus());

    // And push it
    Application::activitiesStack.push_back(activity);
}

void Application::onWindowSizeChanged()
{
    Logger::debug("Layout triggered");

    for (Activity* activity : Application::activitiesStack)
        activity->onWindowSizeChanged();

    // if (Application::background)
    // {
    //     Application::background->setBoundaries(
    //         0,
    //         0,
    //         Application::contentWidth,
    //         Application::contentHeight);

    //     Application::background->invalidate();
    //     Application::background->onWindowSizeChanged();
    // }

    Application::resizeNotificationManager();
    Application::resizeFramerateCounter();
}

void Application::clear()
{
    for (Activity* activity : Application::activitiesStack)
    {
        activity->willDisappear(true);
        delete activity;
    }

    Application::activitiesStack.clear();
}

Theme Application::getTheme()
{
    if (Application::currentThemeVariant == ThemeVariant::LIGHT)
        return getLightTheme();
    else
        return getDarkTheme();
}

ThemeVariant Application::getThemeVariant()
{
    return Application::currentThemeVariant;
}

int Application::loadFont(const char* fontName, const char* filePath)
{
    return nvgCreateFont(Application::vg, fontName, filePath);
}

int Application::loadFontFromMemory(const char* fontName, void* address, size_t size, bool freeData)
{
    return nvgCreateFontMem(Application::vg, fontName, (unsigned char*)address, size, freeData);
}

int Application::findFont(const char* fontName)
{
    return nvgFindFont(Application::vg, fontName);
}

void Application::crash(std::string text)
{
    /* CrashFrame* crashFrame = new CrashFrame(text);
    Application::pushView(crashFrame); TODO: restore */
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
    return Application::vg;
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

/*FramerateCounter::FramerateCounter()
    : Label(LabelStyle::LIST_ITEM, "FPS: ---")
{
    this->setColor(nvgRGB(255, 255, 255));
    this->setVerticalAlign(NVG_ALIGN_MIDDLE);
    this->setHorizontalAlign(NVG_ALIGN_RIGHT);
    this->setBackground(ViewBackground::BACKDROP);

    this->lastSecond = cpu_features_get_time_usec() / 1000; TODO: restore that
}*/

/*void FramerateCounter::frame(FrameContext* ctx)
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

    this->frames++; TODO: restore that

    // Regular frame
    Label::frame(ctx); TODO: restore that
}*/

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

// void Application::setBackground(Background* background)
// {
//     if (Application::background)
//     {
//         Application::background->willDisappear();
//         delete Application::background;
//     }

//     Application::background = background;

//     background->setBoundaries(0, 0, Application::contentWidth, Application::contentHeight);
//     background->invalidate();
//     background->willAppear(true);
// }

void Application::cleanupNvgGlState()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
}

bool Application::XMLViewsRegisterContains(std::string name)
{
    return Application::xmlViewsRegister.count(name) > 0;
}

XMLViewCreator Application::getXMLViewCreator(std::string name)
{
    return Application::xmlViewsRegister[name];
}

void Application::registerBuiltInXMLViews()
{
    Application::registerXMLView("brls:Box", Box::create);
    Application::registerXMLView("brls:Rectangle", Rectangle::create);
    Application::registerXMLView("brls:AppletFrame", AppletFrame::create);
    Application::registerXMLView("brls:Label", Label::create);
    Application::registerXMLView("brls:TabFrame", TabFrame::create);
    Application::registerXMLView("brls:Sidebar", Sidebar::create);
    Application::registerXMLView("brls:Header", Header::create);
    Application::registerXMLView("brls:ScrollingFrame", ScrollingFrame::create);
    Application::registerXMLView("brls:Image", Image::create);
    Application::registerXMLView("brls:Padding", Padding::create);
    Application::registerXMLView("brls:Button", Button::create);
}

void Application::registerXMLView(std::string name, XMLViewCreator creator)
{
    Application::xmlViewsRegister[name] = creator;
}

} // namespace brls
