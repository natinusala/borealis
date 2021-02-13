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

namespace brls
{

TapGestureRecognizer::TapGestureRecognizer(TapGestureRespond respond, bool callbackOnEndOnly)
    : respond(respond), callbackOnEndOnly(callbackOnEndOnly)
{
}

GestureState TapGestureRecognizer::recognitionLoop(TouchState touch, View* view) 
{
    if (!enabled) return GestureState::FAILED;
    
    if (touch.state != TouchEvent::START) 
    {
        if (this->state == GestureState::INTERRUPTED ||
            this->state == GestureState::FAILED) 
        {
            if (respond && !this->callbackOnEndOnly && this->state != lastState) 
                this->respond(this);

            lastState = this->state;
            return this->state;
        }
    }

    switch (touch.state)
    {
    case TouchEvent::START:
        this->state = GestureState::UNSURE;
        this->x = touch.x;
        this->y = touch.y;

        if (respond && !this->callbackOnEndOnly) 
            this->respond(this);
        break;
    case TouchEvent::STAY:
        if (touch.x < view->getX() || touch.x > view->getX() + view->getWidth() ||
            touch.y < view->getY() || touch.y > view->getY() + view->getHeight())
        {
            this->state = GestureState::FAILED;
            if (respond && !this->callbackOnEndOnly) 
                this->respond(this);
        }
        break;
    case TouchEvent::END:
        this->state = GestureState::END;
        if (respond) 
            this->respond(this);
        break;
    case TouchEvent::NONE:
        this->state = GestureState::FAILED;
        break;
    }

    lastState = this->state;
    return this->state;
}

};