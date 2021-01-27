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

#include <borealis/core/logger.hpp>
#include <borealis/platforms/glfw/glfw_input.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace brls
{

#define GLFW_GAMEPAD_BUTTON_NONE SIZE_MAX
#define GLFW_GAMEPAD_BUTTON_MAX 15

// LT and RT do not exist here because they are axes
static const size_t GLFW_BUTTONS_MAPPING[GLFW_GAMEPAD_BUTTON_MAX] = {
    BUTTON_A, // GLFW_GAMEPAD_BUTTON_A
    BUTTON_B, // GLFW_GAMEPAD_BUTTON_B
    BUTTON_X, // GLFW_GAMEPAD_BUTTON_X
    BUTTON_Y, // GLFW_GAMEPAD_BUTTON_Y
    BUTTON_LB, // GLFW_GAMEPAD_BUTTON_LEFT_BUMPER
    BUTTON_RB, // GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER
    BUTTON_BACK, // GLFW_GAMEPAD_BUTTON_BACK
    BUTTON_START, // GLFW_GAMEPAD_BUTTON_START
    BUTTON_GUIDE, // GLFW_GAMEPAD_BUTTON_GUIDE
    BUTTON_LSB, // GLFW_GAMEPAD_BUTTON_LEFT_THUMB
    BUTTON_RSB, // GLFW_GAMEPAD_BUTTON_RIGHT_THUMB
    BUTTON_UP, // GLFW_GAMEPAD_BUTTON_DPAD_UP
    BUTTON_RIGHT, // GLFW_GAMEPAD_BUTTON_DPAD_RIGHT
    BUTTON_DOWN, // GLFW_GAMEPAD_BUTTON_DPAD_DOWN
    BUTTON_LEFT, // GLFW_GAMEPAD_BUTTON_DPAD_LEFT
};

static const size_t GLFW_GAMEPAD_TO_KEYBOARD[GLFW_GAMEPAD_BUTTON_MAX] = {
    GLFW_KEY_ENTER, // GLFW_GAMEPAD_BUTTON_A
    GLFW_KEY_BACKSPACE, // GLFW_GAMEPAD_BUTTON_B
    GLFW_GAMEPAD_BUTTON_NONE, // GLFW_GAMEPAD_BUTTON_X
    GLFW_GAMEPAD_BUTTON_NONE, // GLFW_GAMEPAD_BUTTON_Y
    GLFW_GAMEPAD_BUTTON_NONE, // GLFW_GAMEPAD_BUTTON_LEFT_BUMPER
    GLFW_GAMEPAD_BUTTON_NONE, // GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER
    GLFW_KEY_F1, // GLFW_GAMEPAD_BUTTON_BACK
    GLFW_KEY_ESCAPE, // GLFW_GAMEPAD_BUTTON_START
    GLFW_GAMEPAD_BUTTON_NONE, // GLFW_GAMEPAD_BUTTON_GUIDE
    GLFW_GAMEPAD_BUTTON_NONE, // GLFW_GAMEPAD_BUTTON_LEFT_THUMB
    GLFW_GAMEPAD_BUTTON_NONE, // GLFW_GAMEPAD_BUTTON_RIGHT_THUMB
    GLFW_KEY_UP, // GLFW_GAMEPAD_BUTTON_DPAD_UP
    GLFW_KEY_RIGHT, // GLFW_GAMEPAD_BUTTON_DPAD_RIGHT
    GLFW_KEY_DOWN, // GLFW_GAMEPAD_BUTTON_DPAD_DOWN
    GLFW_KEY_LEFT, // GLFW_GAMEPAD_BUTTON_DPAD_LEFT
};

static void glfwJoystickCallback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        Logger::info("glfw: joystick {} connected", jid);

        if (glfwJoystickIsGamepad(jid))
            Logger::info("glfw: joystick {} is gamepad: \"{}\"", jid, glfwGetGamepadName(jid));
    }
    else if (event == GLFW_DISCONNECTED)
    {
        Logger::info("glfw: joystick {} disconnected", jid);
    }
}

GLFWInputManager::GLFWInputManager(GLFWwindow* window)
    : window(window)
{
    glfwSetJoystickCallback(glfwJoystickCallback);

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        Logger::info("glfw: joystick {} connected", GLFW_JOYSTICK_1);
        Logger::info("glfw: joystick {} is gamepad: \"{}\"", GLFW_JOYSTICK_1, glfwGetGamepadName(GLFW_JOYSTICK_1));
    }
}

void GLFWInputManager::getControllerState(ControllerState* state)
{
    // Get gamepad state
    GLFWgamepadstate glfwState = {};
    glfwGetGamepadState(GLFW_JOYSTICK_1, &glfwState);

    for (size_t i = 0; i < GLFW_GAMEPAD_BUTTON_MAX; i++)
    {
        // Add keyboard keys on top of gamepad buttons
        size_t key = GLFW_GAMEPAD_TO_KEYBOARD[i];

        if (key != GLFW_GAMEPAD_BUTTON_NONE)
            glfwState.buttons[i] |= glfwGetKey(this->window, key);

        // Translate GLFW gamepad to borealis controller
        size_t brlsButton          = GLFW_BUTTONS_MAPPING[i];
        state->buttons[brlsButton] = (bool)glfwState.buttons[i];
    }
}

void GLFWInputManager::updateInputs()
{
}

};
