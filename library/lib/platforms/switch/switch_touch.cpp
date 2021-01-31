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
#include <borealis/platforms/switch/switch_touch.hpp>

namespace brls
{

SwitchTouchManager::SwitchTouchManager()
{

}

void SwitchTouchManager::updateTouchState(TouchState* state)
{
    // Get gamepad state
    double x = oldTouch.x;
    double y = oldTouch.y;
    
    HidTouchScreenState hidState={0};
    if (hidGetTouchScreenStates(&hidState, 1)) 
    {
        if (hidState.count > 0) {
            x = hidState.touches[0].x;
            y = hidState.touches[0].y;
        }
    }
    
    state->x = x / Application::windowScale;
    state->y = y / Application::windowScale;

    if (hidState.count > 0) {
        if (oldTouch.state == START || oldTouch.state == STAY) {
            state->state = STAY;
        } else {
            state->state = START;
        }
    } else {
        if (oldTouch.state == END || oldTouch.state == NONE) {
            state->state = NONE;
        } else {
            state->state = END;
        }
    }
    oldTouch = *state;
}

};