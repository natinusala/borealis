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

static void windowFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (!width || !height)
        return;

    printf("Window size changed to %dx%d\n", width, height);
    glViewport(0, 0, width, height);
}

static void joystickCallback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        printf("Joystick %d connected\n", jid);
        if (glfwJoystickIsGamepad(jid))
            printf("Joystick %d is gamepad: \"%s\"\n", jid, glfwGetGamepadName(jid));
    }
    else if (event == GLFW_DISCONNECTED)
        printf("Joystick %d disconnected\n", jid);
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
        printf("Failed to initialize glfw\n");
        return false;
    }

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Application::window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window)
    {
        printf("glfw: failed to create window");
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

    printf("GL Vendor: %s\n", glGetString(GL_VENDOR));
    printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("GL Version: %s\n", glGetString(GL_VERSION));

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        GLFWgamepadstate state;
        printf("Gamepad detected: %s\n", glfwGetGamepadName(GLFW_JOYSTICK_1));
        glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
    }

    // Initialize the scene
    Application::vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    if (!vg)
    {
        printf("Unable to init nanovg\n");
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
            printf("Using Switch shared font\n");
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
    // TODO: Backport wiggly font fix from RA

    // Load theme
#ifdef __SWITCH__
    ColorSetId theme;
    setsysGetColorSetId(&theme);

    if (theme == ColorSetId_Light)
        Application::currentTheme = &themeLight;
    else
        Application::currentTheme = &themeDark;
#else
    Application::currentTheme = &themeLight; // fight me
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

void Application::onGamepadButtonPressed(char button)
{
    switch (button)
    {
        // Exit by pressing Start (aka Plus)
        case GLFW_GAMEPAD_BUTTON_START:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
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
    for (int i = Application::viewStack.size()-1; i >= 0; i--)
    {
        View *view = Application::viewStack[i];
        viewsToDraw.push_back(view);

        if (!view->isTranslucent())
            break;
    }

    reverse(viewsToDraw.begin(), viewsToDraw.end());

    for (View *view : viewsToDraw)
        view->frame(&frameContext);

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

void Application::pushView(View *view)
{
    view->setBoundaries(0, 0, Application::windowWidth, Application::windowHeight);
    view->willAppear();
    Application::requestFocus(view, FocusDirection::NONE);

    Application::viewStack.push_back(view);
}

void Application::onWindowSizeChanged()
{
    printf("Layout triggered\n");

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