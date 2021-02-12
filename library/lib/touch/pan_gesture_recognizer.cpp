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

PanGestureRecognizer::PanGestureRecognizer(PanGestureRespond respond, PanAxis axis)
    : respond(respond), axis(axis)
{
}

GestureState PanGestureRecognizer::recognitionLoop(TouchState touch, View* view) 
{
    if (!enabled) return GestureState::FAILED;
    
    if (touch.state != TouchEvent::START) 
    {
        if (this->state == GestureState::INTERRUPTED ||
            this->state == GestureState::FAILED) 
            return this->state;
    }

    switch (touch.state)
    {
    case TouchEvent::START:
        this->state = GestureState::UNSURE;
        this->startX = touch.x;
        this->startY = touch.y;
        this->x = touch.x;
        this->y = touch.y;
        break;
    case TouchEvent::STAY:
    case TouchEvent::END:

        this->deltaX = touch.x - this->x;
        this->deltaY = touch.y - this->y;

        this->x = touch.x;
        this->y = touch.y;

        if (this->state == GestureState::UNSURE) 
        {
            if ((fabs(this->startX - touch.x) > MAX_DELTA_MOVEMENT || fabs(this->startY - touch.y) > MAX_DELTA_MOVEMENT))
            {
                switch (axis)
                {
                case PanAxis::HORIZONTAL:
                    if (fabs(deltaX) > fabs(deltaY))
                        this->state = GestureState::START;
                    break;
                case PanAxis::VERTICAL:
                    if (fabs(deltaX) < fabs(deltaY))
                        this->state = GestureState::START;
                    break;
                case PanAxis::NONE:
                    this->state = GestureState::START;
                    break;
                }
            }
        }
        else
        {
            if (touch.state == TouchEvent::STAY)
                this->state = GestureState::STAY;
            else
                this->state = GestureState::END;
        }

        if (this->state == GestureState::START ||
            this->state == GestureState::STAY ||
            this->state == GestureState::END) 
        {
            this->respond(this);
        }
        break;
    case TouchEvent::NONE:
        this->state = GestureState::FAILED;
        break;
    }

    return this->state;
}

};