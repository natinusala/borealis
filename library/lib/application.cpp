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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <borealis.hpp>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad.h>

#define GLM_FORCE_PURE
#define GLM_ENABLE_EXPERIMENTAL
#include <nanovg.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#ifdef __SWITCH__
#include <switch.h>
#endif

#include <chrono>
#include <thread>

// Constants used for scaling as well as
// creating a window of the right size on PC
constexpr uint32_t WINDOW_WIDTH    = 1280;
constexpr uint32_t WINDOW_HEIGHT   = 720;
constexpr const char* WINDOW_TITLE = WINDOW_NAME;

#define DEFAULT_FPS 60
#define BUTTON_REPEAT_DELAY 15
#define BUTTON_REPEAT_CADENCY 5

// glfw code from the glfw hybrid app by fincs
// https://github.com/fincs/hybrid_app

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

    Logger::info("Window size changed to %dx%d", width, height);
    Logger::info("New scale factor is %f", Application::windowScale);
}

static void joystickCallback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        Logger::info("Joystick %d connected", jid);
        if (glfwJoystickIsGamepad(jid))
            Logger::info("Joystick %d is gamepad: \"%s\"", jid, glfwGetGamepadName(jid));
    }
    else if (event == GLFW_DISCONNECTED)
        Logger::info("Joystick %d disconnected", jid);
}

static void errorCallback(int errorCode, const char* description)
{
    Logger::error("[GLFW:%d] %s", errorCode, description);
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

bool Application::init()
{
    return Application::init(Style::horizon(), Theme::horizon());
}

bool Application::init(Style style, Theme theme)
{
    // Init static variables
    Application::currentStyle = style;
    Application::currentFocus = nullptr;
    Application::oldGamepad   = {};
    Application::gamepad      = {};

    // Init theme to defaults
    Application::setTheme(theme);

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

    Application::window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window)
    {
        Logger::error("glfw: failed to create window\n");
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

    Logger::info("GL Vendor: %s", glGetString(GL_VENDOR));
    Logger::info("GL Renderer: %s", glGetString(GL_RENDERER));
    Logger::info("GL Version: %s", glGetString(GL_VERSION));

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        GLFWgamepadstate state;
        Logger::info("Gamepad detected: %s", glfwGetGamepadName(GLFW_JOYSTICK_1));
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
        Result rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
        if (R_SUCCEEDED(rc))
        {
            Logger::info("Using Switch shared font");
            Application::fontStash.regular = nvgCreateFontMem(Application::vg, "regular", (unsigned char*)font.address, font.size, 0);
        }

        rc = plGetSharedFontByType(&font, PlSharedFontType_NintendoExt);
        if (R_SUCCEEDED(rc))
        {
            Logger::info("Using Switch shared symbols font");
            Application::fontStash.sharedSymbols = nvgCreateFontMem(Application::vg, "symbols", (unsigned char*)font.address, font.size, 0);
        }
    }
#else
    // Use illegal font if available
    if (access(ASSET("Illegal-Font.ttf"), F_OK) != -1)
        Application::fontStash.regular = nvgCreateFont(Application::vg, "regular", ASSET("Illegal-Font.ttf"));
    else
        Application::fontStash.regular = nvgCreateFont(Application::vg, "regular", ASSET("inter/Inter-Switch.ttf"));

    if (access(ASSET("Wingdings.ttf"), F_OK) != -1)
        Application::fontStash.sharedSymbols = nvgCreateFont(Application::vg, "sharedSymbols", ASSET("Wingdings.ttf"));
#endif

    // Material font
    if (access(ASSET("material/MaterialIcons-Regular.ttf"), F_OK) != -1)
        Application::fontStash.material = nvgCreateFont(Application::vg, "material", ASSET("material/MaterialIcons-Regular.ttf"));

    // Set symbols font as fallback
    if (Application::fontStash.sharedSymbols)
    {
        Logger::info("Using shared symbols font");
        nvgAddFallbackFontId(Application::vg, Application::fontStash.regular, Application::fontStash.sharedSymbols);
    }
    else
    {
        Logger::error("Shared symbols font not found");
    }

    // Set Material as fallback
    if (Application::fontStash.material)
    {
        Logger::info("Using Material font");
        nvgAddFallbackFontId(Application::vg, Application::fontStash.regular, Application::fontStash.material);
    }
    else
    {
        Logger::error("Material font not found");
    }

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

    // Init window size
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    Application::windowWidth  = viewport[2];
    Application::windowHeight = viewport[3];

    // Init animations engine
    menu_animation_init();

    // Init tasks manager
    Application::taskManager = new TaskManager();

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

    // glfw events
    bool is_active;
    do
    {
        is_active = !glfwGetWindowAttrib(Application::window, GLFW_ICONIFIED);
        if (is_active)
            glfwPollEvents();
        else
            glfwWaitEvents();

        if (glfwWindowShouldClose(Application::window))
        {
            Application::exit();
            return false;
        }
    } while (!is_active);

    // Gamepad
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &Application::gamepad))
    {
        // Gamepad not available, so let's fake it with keyboard
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]  = glfwGetKey(window, GLFW_KEY_LEFT);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = glfwGetKey(window, GLFW_KEY_RIGHT);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]    = glfwGetKey(window, GLFW_KEY_UP);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]  = glfwGetKey(window, GLFW_KEY_DOWN);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_START]      = glfwGetKey(window, GLFW_KEY_ESCAPE);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_BACK]       = glfwGetKey(window, GLFW_KEY_F1);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_A]          = glfwGetKey(window, GLFW_KEY_ENTER);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_B]          = glfwGetKey(window, GLFW_KEY_BACKSPACE);
    }

    // Trigger gamepad events
    // TODO: Translate axis events to dpad events here

    bool anyButtonPressed = false;
    bool repeating        = false;
    static int repeatingButtonTimer = 0;
    static int buttonPressTime = 0;

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
        repeatingButtonTimer++;
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
    if (Application::frameTime > 0.0f)
    {
        retro_time_t currentFrameTime = cpu_features_get_time_usec() - frameStart;
        retro_time_t frameTime        = (retro_time_t)(Application::frameTime * 1000);

        if (frameTime > currentFrameTime)
        {
            retro_time_t toSleep = frameTime - currentFrameTime - 1000; // never sleep too much
            std::this_thread::sleep_for(std::chrono::microseconds(toSleep));
        }
    }

    return true;
}

void Application::quit()
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Application::onGamepadButtonPressed(char button, bool repeating)
{
    if (Application::blockInputsTokens != 0)
        return;

    if (repeating && Application::repetitionOldFocus == Application::currentFocus)
        return;
    
    Application::repetitionOldFocus = Application::currentFocus;

    switch (button)
    {
        // Exit by pressing Start (aka Plus)
        case GLFW_GAMEPAD_BUTTON_START:
            Application::quit();
            break;
        case GLFW_GAMEPAD_BUTTON_BACK:
            Application::toggleFramerateDisplay();
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:
            if (Application::currentFocus && Application::currentFocus->getParent())
                Application::requestFocus(Application::currentFocus->getParent(), FocusDirection::DOWN);
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_UP:
            if (Application::currentFocus && Application::currentFocus->getParent())
                Application::requestFocus(Application::currentFocus->getParent(), FocusDirection::UP);
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:
            if (Application::currentFocus && Application::currentFocus->getParent())
                Application::requestFocus(Application::currentFocus->getParent(), FocusDirection::LEFT);
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:
            if (Application::currentFocus && Application::currentFocus->getParent())
                Application::requestFocus(Application::currentFocus->getParent(), FocusDirection::RIGHT);
            break;
        case GLFW_GAMEPAD_BUTTON_A:
            if (Application::currentFocus)
                Application::currentFocus->click();
            break;
        case GLFW_GAMEPAD_BUTTON_B:
            if (Application::currentFocus)
                Application::currentFocus->cancel();
            break;
        default:
            break;
    }
}

void Application::frame()
{
    // Frame context
    FrameContext frameContext = FrameContext();

    frameContext.pixelRatio = (float)Application::windowWidth / (float)Application::windowHeight;
    frameContext.vg         = Application::vg;
    frameContext.fontStash  = &Application::fontStash;
    frameContext.theme      = Application::getThemeValues();

    nvgBeginFrame(Application::vg, Application::windowWidth, Application::windowHeight, frameContext.pixelRatio);
    nvgScale(Application::vg, Application::windowScale, Application::windowScale);

    // GL Clear
    glClearColor(
        frameContext.theme->backgroundColor[0],
        frameContext.theme->backgroundColor[1],
        frameContext.theme->backgroundColor[2],
        1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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

    // End frame
    nvgResetTransform(Application::vg); // scale
    nvgEndFrame(Application::vg);
}

void Application::exit()
{
    Application::clear();

    if (Application::vg)
        nvgDeleteGL3(Application::vg);

    glfwTerminate();

    menu_animation_free();

    if (Application::taskManager)
        delete Application::taskManager;

    if (Application::framerateCounter)
        delete Application::framerateCounter;
}

void Application::setDisplayFramerate(bool enabled)
{
    if (!Application::framerateCounter && enabled)
    {
        Logger::info("Enabling framerate counter");
        Application::framerateCounter = new FramerateCounter();
        Application::resizeFramerateCounter();
    }
    else if (Application::framerateCounter && !enabled)
    {
        Logger::info("Disabling framerate counter");
        delete Application::framerateCounter;
        Application::framerateCounter = nullptr;
    }
}

void Application::toggleFramerateDisplay()
{
    Application::setDisplayFramerate(!Application::framerateCounter);
}

void Application::resizeFramerateCounter()
{
    if (!Application::framerateCounter)
        return;

    Style* style                   = Application::getStyle();
    unsigned framerateCounterWidth = style->FramerateCounter.width;
    unsigned width                 = WINDOW_WIDTH;

    Application::framerateCounter->setBoundaries(
        width - framerateCounterWidth,
        0,
        framerateCounterWidth,
        style->FramerateCounter.height);
    Application::framerateCounter->invalidate();
}

void Application::requestFocus(View* view, FocusDirection direction)
{
    View* oldFocus = Application::currentFocus;
    View* newFocus = view->requestFocus(direction, oldFocus);

    if (oldFocus != newFocus && newFocus)
    {
        if (oldFocus)
            oldFocus->onFocusLost();
        newFocus->onFocusGained();

        Application::currentFocus = newFocus;
    }
    else if (oldFocus)
        oldFocus->shakeHighlight(direction);
}

void Application::popView(ViewAnimation animation)
{
    if (Application::viewStack.size() == 0)
        return;

    Application::blockInputs();

    View* last = Application::viewStack[Application::viewStack.size() - 1];
    last->willDisappear();

    last->setForceTranslucent(true);

    bool wait = animation == ViewAnimation::FADE; // wait for the new view animation to be done before showing the old one?

    // Hide animation (and show previous view, if any)
    last->hide([last, animation, wait]() {
        last->setForceTranslucent(false);
        Application::viewStack.pop_back();
        delete last;

        // Animate the old view once the old one
        // has ended its animation
        if (Application::viewStack.size() > 0 && wait)
        {
            View* newLast = Application::viewStack[Application::viewStack.size() - 1];

            if (newLast->isHidden())
                newLast->show([]() {}, true, animation);
        }

        Application::unblockInputs();
    },
        true, animation);

    // Animate the old view immediately
    if (!wait && Application::viewStack.size() > 1)
    {
        View* toShow = Application::viewStack[Application::viewStack.size() - 2];
        toShow->show([]() {}, true, animation);
    }

    // Focus
    if (Application::focusStack.size() > 0)
    {
        View* newFocus = Application::focusStack[Application::focusStack.size() - 1];

        Logger::debug("Giving focus to %s, and removing it from the focus stack", newFocus->name().c_str());

        Application::requestFocus(newFocus, FocusDirection::NONE);
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

    view->setBoundaries(0, 0, Application::contentWidth, Application::contentHeight);

    if (!fadeOut)
        view->show([]() { Application::unblockInputs(); }, true, animation);
    else
        view->alpha = 0.0f;

    // Focus
    if (Application::viewStack.size() > 0)
    {
        Logger::debug("Pushing %s to the focus stack", Application::currentFocus->name().c_str());
        Application::focusStack.push_back(Application::currentFocus);
    }

    view->willAppear();
    Application::requestFocus(view, FocusDirection::NONE);

    Application::viewStack.push_back(view);
}

void Application::onWindowSizeChanged()
{
    Logger::debug("Layout triggered");

    for (View* view : Application::viewStack)
    {
        view->setBoundaries(0, 0, Application::contentWidth, Application::contentHeight);
        view->invalidate();
    }

    Application::resizeFramerateCounter();
}

void Application::clear()
{
    for (View* view : Application::viewStack)
    {
        view->willDisappear();
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
    Application::blockInputsTokens -= 1;

    if (Application::blockInputsTokens < 0)
        Application::blockInputsTokens = 0;
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

FramerateCounter::FramerateCounter()
    : Label(LabelStyle::LIST_ITEM, "FPS: ---")
{
    this->setColor(nvgRGB(255, 255, 255));
    this->setVerticalAlign(NVG_ALIGN_MIDDLE);
    this->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->setBackground(Background::BACKDROP);

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

    Logger::info("Maximum FPS set to %d - using a frame time of %.2f ms", fps, Application::frameTime);
}

} // namespace brls
