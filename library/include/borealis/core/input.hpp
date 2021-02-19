/*
    Copyright 2021 natinusala

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

namespace brls
{

// Abstract buttons enum - names correspond to a generic Xbox controller
// LT and RT should not be buttons but for the sake of simplicity we'll assume they are.
// Similarly, DPAD (also called HAT) is assumed to be buttons here.
enum ControllerButton
{
    BUTTON_LT = 0,
    BUTTON_LB,

    BUTTON_LSB,

    BUTTON_UP,
    BUTTON_RIGHT,
    BUTTON_DOWN,
    BUTTON_LEFT,

    BUTTON_BACK,
    BUTTON_GUIDE,
    BUTTON_START,

    BUTTON_RSB,

    BUTTON_Y,
    BUTTON_B,
    BUTTON_A,
    BUTTON_X,

    BUTTON_RB,
    BUTTON_RT,

    _BUTTON_MAX,
};

// Abstract axis enum - names correspond to a generic Xbox controller
enum ControllerAxis
{
    LEFT_X,
    LEFT_Y,

    // No Z axis, LT and RT are in the buttons enum for the sake of simplicity

    RIGHT_X, // also called 5th axis
    RIGHT_Y, // also called 4th axis

    _AXES_MAX,
};

// Represents the state of the controller (a gamepad or a keyboard) in the current frame
typedef struct ControllerState
{
    bool buttons[_BUTTON_MAX]; // true: pressed
    float axes[_AXES_MAX]; // from 0.0f to 1.0f
} ControllerState;

// Interface responsible for reporting input state to the application - button presses,
// axis position and touch screen state
class InputManager
{
  public:
    virtual ~InputManager() { }

    /**
     * Called once every frame to fill the given ControllerState struct with the controller state.
     */
    virtual void updateControllerState(ControllerState* state) = 0;
};

}; // namespace brls
