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

#include <borealis/platform_driver/platform_driver_glfw.hpp>
#include <borealis/logger.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

namespace brls::drv {

    bool PlatformDriverGLFW::initialize(const std::string &title, unsigned int windowWidth, unsigned int windowHeight) {
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

        this->window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), nullptr, nullptr);

        if (!window)
        {
            Logger::error("glfw: failed to create window\n");
            glfwTerminate();
            return false;
        }

         // Configure window
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwMakeContextCurrent(window);
        //glfwSetFramebufferSizeCallback(window, windowFramebufferSizeCallback);
        glfwSetKeyCallback(window, windowKeyCallback);
        glfwSetJoystickCallback(joystickCallback);

        // Load OpenGL routines using glad
        //gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        glfwSwapInterval(1);

        /*Logger::info("GL Vendor: %s", glGetString(GL_VENDOR));
        Logger::info("GL Renderer: %s", glGetString(GL_RENDERER));
        Logger::info("GL Version: %s", glGetString(GL_VERSION));*/

        if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
        {
            GLFWgamepadstate state;
            Logger::info("Gamepad detected: %s", glfwGetGamepadName(GLFW_JOYSTICK_1));
            glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
        }

        // Initialize the scene
        this->vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
        if (!this->vg)
        {
            Logger::error("Unable to init nanovg");
            glfwTerminate();
            return false;
        }

        glfwSetTime(0.0);

        // Init window size
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        this->windowWidth  = viewport[2];
        this->windowHeight = viewport[3];

        return true;
    }

    bool PlatformDriverGLFW::exit() {
        if (this->vg)
            nvgDeleteGL3(this->vg);

        glfwTerminate();
    }

    bool PlatformDriverGLFW::update() {
        // glfw events
        bool is_active;
        do
        {
            is_active = !glfwGetWindowAttrib(this->window, GLFW_ICONIFIED);
            if (is_active)
                glfwPollEvents();
            else
                glfwWaitEvents();

            if (glfwWindowShouldClose(this->window))
            {
                this->exit();
                return false;
            }
        } while (!is_active);

        // Gamepad
        GLFWgamepadstate gamepad;
        if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
        {
            // Keyboard -> DPAD Mapping
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]    = glfwGetKey(window, GLFW_KEY_LEFT);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]   = glfwGetKey(window, GLFW_KEY_RIGHT);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]      = glfwGetKey(window, GLFW_KEY_UP);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]    = glfwGetKey(window, GLFW_KEY_DOWN);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_START]        = glfwGetKey(window, GLFW_KEY_ESCAPE);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_BACK]         = glfwGetKey(window, GLFW_KEY_F1);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_A]            = glfwGetKey(window, GLFW_KEY_ENTER);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_B]            = glfwGetKey(window, GLFW_KEY_BACKSPACE);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_X]            = glfwGetKey(window, GLFW_KEY_X);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_Y]            = glfwGetKey(window, GLFW_KEY_Y);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB]   = glfwGetKey(window, GLFW_KEY_Q);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB]  = glfwGetKey(window, GLFW_KEY_E);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]  = glfwGetKey(window, GLFW_KEY_L);
            gamepad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = glfwGetKey(window, GLFW_KEY_R);
        }

        this->gamepadDownOld = this->gamepadDown;

        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) << static_cast<unsigned short>(Key::DLEFT);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) << static_cast<unsigned short>(Key::DRIGHT);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) << static_cast<unsigned short>(Key::DUP);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) << static_cast<unsigned short>(Key::DDOWN);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_START]) << static_cast<unsigned short>(Key::PLUS);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_BACK]) << static_cast<unsigned short>(Key::MINUS);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_A]) << static_cast<unsigned short>(Key::A);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_B]) << static_cast<unsigned short>(Key::B);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_X]) << static_cast<unsigned short>(Key::X);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_Y]) << static_cast<unsigned short>(Key::Y);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB]) << static_cast<unsigned short>(Key::LSTICK);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB]) << static_cast<unsigned short>(Key::RSTICK);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]) << static_cast<unsigned short>(Key::L);
        this->gamepadDown |= (!!gamepad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER]) << static_cast<unsigned short>(Key::R);

        return true;
    }

    void PlatformDriverGLFW::frame() {
        // GL Clear
        glClearColor(
            frameContext.theme->backgroundColor[0],
            frameContext.theme->backgroundColor[1],
            frameContext.theme->backgroundColor[2],
            1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void PlatformDriverGLFW::swapBuffers() {
        glfwSwapBuffers(this->window);
    }

    bool PlatformDriverGLFW::isAnyKeyDown() const {
        return this->gamepadDown > 0;
    }

    bool PlatformDriverGLFW::haveKeyStatesChanged() const {
        return this->gamepadDown != this->gamepadDownOld;
    }

    bool PlatformDriverGLFW::isKeyDown(const Key &key) const {
        return (this->gamepadDown & static_cast<unsigned short>(key)) &&
               !(this->gamepadDownOld & static_cast<unsigned short>(key));
    }

    bool PlatformDriverGLFW::isKeyUp(const Key &key) const {
        return (this->gamepadDownOld & static_cast<unsigned short>(key)) &&
               !(this->gamepadDown & static_cast<unsigned short>(key));
    }

    bool PlatformDriverGLFW::isKeyHeld(const Key &key) const {
        return !!(this->gamepadDownOld & static_cast<unsigned short>(key));
    }

    std::pair<int, int> PlatformDriverGLFW::getTouchPosition() const {
        return { 0, 0 }; 
    }

    int PlatformDriverGLFW::getTouchCount() const {
        return 0;
    }

    void PlatformDriverGLFW::playSoundById(const std::string &title) const {

    }


        // TODO: Use this instead of a glViewport each frame
    /*static void windowFramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        if (!width || !height)
            return;

        glViewport(0, 0, width, height);
        Application::windowScale = (float)width / (float)WINDOW_WIDTH;

        float contentHeight = ((float)height / (Application::windowScale * (float)WINDOW_HEIGHT)) * (float)WINDOW_HEIGHT;

        Application::contentWidth  = WINDOW_WIDTH;
        Application::contentHeight = (unsigned)roundf(contentHeight);

        Application::resizeNotificationManager();

        Logger::info("Window size changed to %dx%d", width, height);
        Logger::info("New scale factor is %f", Application::windowScale);
    }*/

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

} // namespace brls::drv