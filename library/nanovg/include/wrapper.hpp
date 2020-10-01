#pragma once

#include <nanovg.h>

#include "compat_input.hpp"

namespace nvg {

    /**
     * @brief   Initializes NanoVG context and dependencies.
     * @note    Call once on application launch.
    */
    void Initialize();

    /**
     * @brief   Cleans NanoVG context and dependencies up.
     * @note    Call once on applicaiton exit.
    */
    void Exit();

    /**
     * @brief   Updates state and returns if application should continue to run.
     * @note    Don't call while you have a nvg::Context object.
    */
    bool MainLoop();

    /**
     * @brief   Signals the application to exit.
     * @note    MainLoop will return false on the next iteration.
    */
    void Break();

    /**
     * @brief   Returns NanoVG context.
     * @note    During main loop use nvg::Frame.
     * @note    Pointer only valid in between nvg::Initialize & nvg::Exit.
    */
    NVGcontext *GetContext();

    /**
     * @brief   NanoVG frame wrapper
     * @note    Contructor begins frame, destructor ends frame.
     * @note    Can be used in place of NVGcontext.
    */
    class Frame {
        NVGcontext *impl;
        int slot;

      public:
        Frame();
        ~Frame();

        inline operator NVGcontext *() {
            return impl;
        }
    };

}
