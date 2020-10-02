#include <nanovg.h>

#include "glad.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <nvgwrapper.hpp>

namespace nvg {

    namespace {

        GLFWwindow *s_Window = nullptr;
        NVGcontext *s_vg;
        struct {
            int regular, symbols;
        } s_fonts;

        constexpr const float DefaultWindowWidth  = 1280.f;
        constexpr const float DefaultWindowHeight = 720.f;
        constexpr const float DefaultAspectRatio  = DefaultWindowWidth / DefaultWindowHeight;

        uint32_t s_WindowWidth  = DefaultWindowWidth;
        uint32_t s_WindowHeight = DefaultWindowHeight;
        uint32_t s_WindowX = 0;
        uint32_t s_WindowY = 0;
        float s_WindowScale     = 1.f;

        uint64_t kDown, kUp, kHeld;
        uint32_t touchCount = 0;
        TouchState touchState;
        int32_t touchX = 0.f, touchY = 0.f;

        void windowFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
            if (!width || !height)
                return;

            s_WindowX = 0;
            s_WindowY = 0;

            const float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

            if (aspect_ratio < DefaultAspectRatio) {
                s_WindowWidth  = width;
                s_WindowHeight = (float)width / DefaultAspectRatio;

                s_WindowY = (height - s_WindowHeight) / 2.f;
            } else if (aspect_ratio > DefaultAspectRatio) {
                s_WindowWidth  = (float)height * DefaultAspectRatio;
                s_WindowHeight = height;

                s_WindowX = (width - s_WindowWidth) / 2.f;
            } else {
                s_WindowHeight = height;
                s_WindowWidth  = width;
            }

            glViewport(s_WindowX, s_WindowY, s_WindowWidth, s_WindowHeight);
            s_WindowScale = s_WindowWidth / DefaultWindowWidth;
        }

        // clang-format off
        constexpr const std::array<std::pair<int, uint32_t>, 32> KeyMap = {{
            {GLFW_KEY_ENTER, KEY_A},
            {GLFW_KEY_BACKSPACE, KEY_B},
            {GLFW_KEY_X, KEY_X},
            {GLFW_KEY_Z, KEY_Y},
            {GLFW_KEY_ESCAPE, KEY_PLUS},
            {GLFW_KEY_F1, KEY_MINUS},
            {GLFW_KEY_W, KEY_DUP | KEY_LSTICK_UP},
            {GLFW_KEY_S, KEY_DDOWN | KEY_LSTICK_DOWN},
            {GLFW_KEY_A, KEY_DLEFT | KEY_LSTICK_LEFT},
            {GLFW_KEY_D, KEY_DRIGHT | KEY_LSTICK_RIGHT},
            {GLFW_KEY_RIGHT, KEY_RSTICK_LEFT},
            {GLFW_KEY_LEFT, KEY_RSTICK_UP},
            {GLFW_KEY_DOWN, KEY_RSTICK_RIGHT},
            {GLFW_KEY_UP, KEY_RSTICK_DOWN},
        }};
        // clang-format on

        void windowKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                /* Check for toggle-fullscreen combo. */
                if (key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT) {
                    static int saved_x, saved_y, saved_width, saved_height;

                    if (!glfwGetWindowMonitor(window)) {
                        /* Back up window position/size. */
                        glfwGetWindowPos(window, &saved_x, &saved_y);
                        glfwGetWindowSize(window, &saved_width, &saved_height);

                        /* Switch to fullscreen mode. */
                        GLFWmonitor *monitor    = glfwGetPrimaryMonitor();
                        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
                        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                    } else {
                        /* Switch back to windowed mode. */
                        glfwSetWindowMonitor(window, nullptr, saved_x, saved_y, saved_width, saved_height, GLFW_DONT_CARE);
                    }
                }
            }

            const auto it = std::find_if(std::cbegin(KeyMap), std::cend(KeyMap), [&](const auto &pair) {
                return pair.first == key;
            });

            if (it != std::cend(KeyMap)) {
                const uint32_t flag = it->second;
                const uint32_t mask = ~flag;
                switch (action) {
                    case GLFW_RELEASE:
                        kDown &= mask;
                        kUp |= flag;
                        kHeld &= mask;
                        break;
                    case GLFW_PRESS:
                        kDown |= flag;
                        kUp &= mask;
                        kHeld &= mask;
                        break;
                }
            }
        }

        void windowMouseCallback(GLFWwindow *window, int button, int action, int mod) {
            switch (action) {
                case GLFW_RELEASE:
                    if (touchCount)
                        touchCount--;
                    break;
                case GLFW_PRESS:
                    touchCount++;
                    break;
            }
        }

    }

    void Initialize() {
        assert(s_Window == nullptr);

        /* Init glfw. */
        glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
        if (!glfwInit()) {
            assert(!"Failed to initialize glfw");
        }

        /* Create window. */
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        s_Window = glfwCreateWindow(s_WindowWidth, s_WindowHeight, "NanoVG Wrapper", nullptr, nullptr);
        assert(s_Window != nullptr);

        /* Configure window. */
        glfwSetInputMode(s_Window, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwMakeContextCurrent(s_Window);
        glfwSetFramebufferSizeCallback(s_Window, windowFramebufferSizeCallback);
        glfwSetKeyCallback(s_Window, windowKeyCallback);
        glfwSetMouseButtonCallback(s_Window, windowMouseCallback);

        /* Load OpenGL routines using glad. */
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        glfwSwapInterval(-1);

        /* Initialize the scene. */
        s_vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
        assert(s_vg && "Unable to init nanovg");

        glfwSetTime(0.0);

        /* Load default font. */
        s_fonts.regular = nvgCreateFont(s_vg, "regular", "fonts/FontStandard.ttf");
        assert(s_fonts.regular != -1);

        /* Load symbol font. */
        s_fonts.symbols = nvgCreateFont(s_vg, "symbols", "fonts/FontNintendoExtended.ttf");
        assert(s_fonts.symbols != -1);

        /* Set symbols font as fallback. */
        nvgAddFallbackFontId(s_vg, s_fonts.regular, s_fonts.symbols);

        /* Init window size. */
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        s_WindowWidth  = viewport[2];
        s_WindowHeight = viewport[3];
    }

    void Exit() {
        assert(s_Window);
        assert(s_vg);

        nvgDeleteGL3(s_vg);
        glfwDestroyWindow(s_Window);
        glfwTerminate();
    }

    bool MainLoop() {
        /* Update inputs. */
        kHeld |= kDown;
        kDown = 0;
        kUp   = 0;
        if (touchCount > 0) {
            double tmpX, tmpY;
            glfwGetCursorPos(s_Window, &tmpX, &tmpY);
            touchX = tmpX / static_cast<double>(s_WindowScale) - (s_WindowX / 2);
            touchY = tmpY / static_cast<double>(s_WindowScale) - (s_WindowY / 2);

            touchX = std::clamp(touchX, 0, 1280);
            touchY = std::clamp(touchY, 0, 720);

            switch (touchState) {
                case TouchState::Touched:
                case TouchState::Held:
                    touchState = TouchState::Held;
                    break;
                default:
                    touchState = TouchState::Touched;
                    break;
            }
        } else {
            switch (touchState) {
                case TouchState::Touched:
                case TouchState::Held:
                    touchState = TouchState::Released;
                    break;
                case TouchState::Released:
                    touchX     = 0;
                    touchY     = 0;
                    touchState = TouchState::None;
                    break;
                default:
                    /* ... */
                    break;
            }
        }

        bool is_active;
        do {
            is_active = !glfwGetWindowAttrib(s_Window, GLFW_ICONIFIED);
            if (is_active) {
                glfwPollEvents();
            } else {
                glfwWaitEvents();
            }
            if (glfwWindowShouldClose(s_Window)) {
                glfwSetWindowShouldClose(s_Window, GLFW_TRUE);
                return false;
            }
        } while (!is_active);

        return !glfwWindowShouldClose(s_Window);
    }

    void Break() {
        glfwSetWindowShouldClose(s_Window, GLFW_TRUE);
    }

    NVGcontext *GetContext() {
        return s_vg;
    }

    Frame::Frame() {
        glClearColor(0.0f, 0.0f, 0.0f, 0.85f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        this->impl = s_vg;
        nvgBeginFrame(s_vg, s_WindowWidth, s_WindowHeight, (float)s_WindowWidth / (float)s_WindowHeight);
        nvgScale(s_vg, s_WindowScale, s_WindowScale);
    }

    Frame::~Frame() {
        nvgResetTransform(s_vg);
        nvgEndFrame(s_vg);

        glfwSwapBuffers(s_Window);
    }

    namespace keys {

        uint64_t Down() {
            return kDown;
        }

        uint64_t Up() {
            return kUp;
        }

        uint64_t Held() {
            return kHeld;
        }

    }

    TouchState GetTouchState() {
        return touchState;
    }

    std::pair<int32_t, int32_t> GetTouchPos() {
        return { touchX, touchY };
    }

}
