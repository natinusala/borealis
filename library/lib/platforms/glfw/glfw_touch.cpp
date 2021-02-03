/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala
    Copyright (C) 2021  XITRIX

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

#include <borealis.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/platforms/glfw/glfw_touch.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace brls
{

GLFWTouchManager::GLFWTouchManager(GLFWwindow* window)
    : window(window)
{

}

void GLFWTouchManager::updateTouchState(TouchState* state)
{
    // Get gamepad state
    double x, y;
    glfwGetCursorPos(this->window, &x, &y);

    state->x = x / Application::windowScale;
    state->y = y / Application::windowScale;

    int clickState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (clickState == GLFW_PRESS) {
        if (oldTouch.state == TouchEvent::START || oldTouch.state == TouchEvent::STAY) {
            state->state = TouchEvent::STAY;
        } else {
            state->state = TouchEvent::START;
        }
    } else {
        if (oldTouch.state == TouchEvent::END || oldTouch.state == TouchEvent::NONE) {
            state->state = TouchEvent::NONE;
        } else {
            state->state = TouchEvent::END;
        }
    }
    oldTouch = *state;
}

};