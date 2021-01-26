/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala

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
     * Called every frame so that the manager can poll the hardware if needed.
     */
    virtual void updateInputs() = 0;

    /**
     * Called to fill the given ControllerState struct with the controller state
     * at the current frame.
     */
    virtual void getControllerState(ControllerState* state) = 0;
};

}; // namespace brls
