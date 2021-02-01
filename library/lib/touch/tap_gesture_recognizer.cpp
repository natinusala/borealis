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

#include <borealis/touch/tap_gesture_recognizer.hpp>

#include <borealis.hpp>

namespace brls
{

TapGestureRecognizer::TapGestureRecognizer(GestureRespond respond, int target)
    : target(target), respond(respond)
{
    this->counter = 0;
}

bool TapGestureRecognizer::recognitionLoop(TouchState touch, bool locked) 
{
    if (!enabled) return false;

    switch (touch.state)
    {
    case START:
        this->success = true;
        this->x = touch.x;
        this->y = touch.y;
        break;
    case STAY:
        if (touch.x != this->x || touch.y != this->y) {
            this->success = false;
            counter = 0;
        }
        break;
    case END:
        if (touch.x == this->x && touch.y == this->y) 
            this->counter++;

        if (counter >= target) 
        {
            if (respond && !locked) 
                respond();
            counter = 0;
        }
        break;
    case NONE:
        this->success = false;
        break;
    }

    return this->success;
}

};