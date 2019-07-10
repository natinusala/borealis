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
#include <string>
#include <algorithm>
#include <unistd.h>

#include <Borealis.hpp>
#include <Animations.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad.h>

#define GLM_FORCE_PURE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#ifdef __SWITCH__
#include <switch.h>
#endif

constexpr uint32_t WINDOW_WIDTH = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;
constexpr const char* WINDOW_TITLE = WINDOW_NAME;

using namespace std;

// glfw code from the glfw hybrid app by fincs
// https://github.com/fincs/hybrid_app

// TODO: Add a framerate counter

// TODO: Use this instead of a glViewport each frame
static void windowFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (!width || !height)
        return;

    info("Window size changed to %dx%d", width, height);
    glViewport(0, 0, width, height);
}

static void joystickCallback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        info("Joystick %d connected", jid);
        if (glfwJoystickIsGamepad(jid))
            info("Joystick %d is gamepad: \"%s\"", jid, glfwGetGamepadName(jid));
    }
    else if (event == GLFW_DISCONNECTED)
        info("Joystick %d disconnected", jid);
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
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
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

bool Application::init(StyleEnum style)
{
    // Init static variables
    setStyle(style);
    Application::currentFocus   = nullptr;
    Application::oldGamepad     = {};
    Application::gamepad        = {};

    // Init glfw
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
    if (!glfwInit())
    {
        error("Failed to initialize glfw");
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
        error("glfw: failed to create window\n");
        glfwTerminate();
        return false;
    }

    // Configure window
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, windowFramebufferSizeCallback);
    glfwSetKeyCallback(window, windowKeyCallback);
    glfwSetJoystickCallback(joystickCallback);

    // Load OpenGL routines using glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    info("GL Vendor: %s", glGetString(GL_VENDOR));
    info("GL Renderer: %s", glGetString(GL_RENDERER));
    info("GL Version: %s", glGetString(GL_VERSION));

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        GLFWgamepadstate state;
        info("Gamepad detected: %s", glfwGetGamepadName(GLFW_JOYSTICK_1));
        glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
    }

    // Initialize the scene
    Application::vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    if (!vg)
    {
        error("Unable to init nanovg");
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
        if(R_SUCCEEDED(rc))
        {
            info("Using Switch shared font");
            Application::fontStash.regular = nvgCreateFontMem(Application::vg, "regular", (unsigned char*)font.address, font.size, 0);
        }
    }
#else
    // Use illegal font if available
    if (access(ASSET("Illegal-Font.ttf"), F_OK) != -1)
        Application::fontStash.regular = nvgCreateFont(Application::vg, "regular", ASSET("Illegal-Font.ttf"));
    else
        Application::fontStash.regular = nvgCreateFont(Application::vg, "regular", ASSET("inter/Inter-Switch.ttf"));
#endif

    // TODO: Load symbols shared font as a fallback
    // TODO: Font Awesome as fallback too?

    // Load theme
#ifdef __SWITCH__
    ColorSetId theme;
    setsysGetColorSetId(&theme);

    if (theme == ColorSetId_Dark)
        Application::currentTheme = &themeDark;
    else
        Application::currentTheme = &themeLight;
#else
    char *themeEnv = getenv("BOREALIS_THEME");
    if (themeEnv != nullptr && !strcasecmp(themeEnv, "DARK"))
        Application::currentTheme = &themeDark;
    else
        Application::currentTheme = &themeLight;
#endif

    // Init window size
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    Application::windowWidth   = viewport[2];
    Application::windowHeight  = viewport[3];

    // Init animations engine
    menu_animation_init();

    return true;
}

bool Application::mainLoop()
{
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
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_A]          = glfwGetKey(window, GLFW_KEY_ENTER);
        Application::gamepad.buttons[GLFW_GAMEPAD_BUTTON_B]          = glfwGetKey(window, GLFW_KEY_BACKSPACE);
    }

    // Trigger gamepad events
    // TODO: Translate axis events to dpad events here
    // TODO: Handle key repetition
    for (int i = GLFW_GAMEPAD_BUTTON_A; i <= GLFW_GAMEPAD_BUTTON_LAST; i++)
    {
        if (Application::gamepad.buttons[i] == GLFW_PRESS && Application::oldGamepad.buttons[i] != GLFW_PRESS)
            Application::onGamepadButtonPressed(i);
    }

    Application::oldGamepad = Application::gamepad;

    // Handle window size changes
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    unsigned newWidth   = viewport[2];
    unsigned newHeight  = viewport[3];

    if (Application::windowWidth != newWidth || Application::windowHeight != newHeight)
    {
        Application::windowWidth   = newWidth;
        Application::windowHeight  = newHeight;
        Application::onWindowSizeChanged();
    }

    // Animations
    menu_animation_update();

    // Render
    Application::frame();
    glfwSwapBuffers(window);

    return true;
}

void Application::quit()
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Application::onGamepadButtonPressed(char button)
{
    if (Application::blockInputsTokens != 0)
        return;

    switch (button)
    {
        // Exit by pressing Start (aka Plus)
        case GLFW_GAMEPAD_BUTTON_START:
            Application::quit();
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

// TODO: Scale properly on docked Switch
void Application::frame()
{
    // Frame context
    FrameContext frameContext = FrameContext();

    frameContext.pixelRatio     = (float)Application::windowWidth / (float)Application::windowHeight;
    frameContext.vg             = Application::vg;
    frameContext.fontStash      = &Application::fontStash;
    frameContext.theme          = Application::currentTheme;

    nvgBeginFrame(Application::vg, Application::windowWidth, Application::windowHeight, frameContext.pixelRatio);

    // GL Clear
    glClearColor(
        Application::currentTheme->backgroundColor[0],
        Application::currentTheme->backgroundColor[1],
        Application::currentTheme->backgroundColor[2],
        1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    vector<View*> viewsToDraw;

    // Draw all views in the stack
    // until we find one that's not translucent
    for (size_t i = 0; i < Application::viewStack.size(); i++)
    {
        View *view = Application::viewStack[Application::viewStack.size()-1-i];
        viewsToDraw.push_back(view);

        if (!view->isTranslucent())
            break;
    }

    for (size_t i = 0; i < viewsToDraw.size(); i++)
    {
        View *view = viewsToDraw[viewsToDraw.size()-1-i];
        view->frame(&frameContext);
    }

    // End frame
    nvgEndFrame(Application::vg);
}

void Application::exit()
{
    Application::clear();

    if (Application::vg)
        nvgDeleteGL3(Application::vg);

    glfwTerminate();

    menu_animation_free();
}

void Application::requestFocus(View *view, FocusDirection direction)
{
    View *oldFocus = Application::currentFocus;
    View *newFocus = view->requestFocus(direction, oldFocus);

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

void Application::popView()
{
    if (Application::viewStack.size() == 0)
        return;

    Application::blockInputs();

    View* last = Application::viewStack[Application::viewStack.size()-1];
    last->willDisappear();

    // Hide animation (and show previous view, if any)
    last->hide([last](){
        Application::viewStack.pop_back();
        delete last;

        if (Application::viewStack.size() > 0)
        {
            View *newLast = Application::viewStack[Application::viewStack.size()-1];

            if (newLast->isHidden())
                newLast->show([](){});
        }

        Application::unblockInputs();
    });

    // Focus
    if (Application::focusStack.size() > 0)
    {
        View *newFocus = Application::focusStack[Application::focusStack.size()-1];

        debug("Giving focus to %s, and removing it from the focus stack", newFocus->name().c_str());

        Application::requestFocus(newFocus, FocusDirection::NONE);
        Application::focusStack.pop_back();
    }
}

void Application::pushView(View *view)
{
    Application::blockInputs();

    // Call hide() on the previous view in the stack if no
    // views are translucent, then call show() once the animation ends
    View* last = nullptr;
    if (Application::viewStack.size() > 0)
        last = Application::viewStack[Application::viewStack.size()-1];

    bool fadeOutAnimation = last && !last->isTranslucent() && !view->isTranslucent();

    // Fade out animation
    if (fadeOutAnimation)
    {
        view->setForceTranslucent(true); // set the new view translucent until the fade out animation is done playing
        last->hide([]() {
            View *newLast = Application::viewStack[Application::viewStack.size()-1];
            newLast->setForceTranslucent(false);
            newLast->show([](){ Application::unblockInputs(); });
        });
    }

    view->setBoundaries(0, 0, Application::windowWidth, Application::windowHeight);

    if (!fadeOutAnimation)
        view->show([](){ Application::unblockInputs(); });
    else
        view->alpha = 0.0f;

    // Focus
    if (Application::viewStack.size() > 0)
    {
        debug("Pushing %s to the focus stack", Application::currentFocus->name().c_str());
        Application::focusStack.push_back(Application::currentFocus);
    }


    view->willAppear();
    Application::requestFocus(view, FocusDirection::NONE);

    Application::viewStack.push_back(view);
}

void Application::onWindowSizeChanged()
{
    debug("Layout triggered");

    for (View *view : Application::viewStack)
    {
        view->setBoundaries(0, 0, Application::windowWidth, Application::windowHeight);
        view->invalidate();
    }
}

void Application::clear()
{
    for (View *view : Application::viewStack)
    {
        view->willDisappear();
        delete view;
    }

    Application::viewStack.clear();
}


void Application::setStyle(StyleEnum style)
{
    switch (style)
    {
        case StyleEnum::ACCURATE:
            Application::currentStyle = &styleAccurate;
    }
}

Style* Application::getStyle()
{
    return Application::currentStyle;
}

Theme* Application::getTheme()
{
    return Application::currentTheme;
}

void Application::crash(string text)
{
    CrashFrame *crashFrame = new CrashFrame(text);
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

NVGcontext* Application::getNVGContext() {
    return Application::vg;
}