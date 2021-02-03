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

bool PanGestureRecognizer::recognitionLoop(TouchState touch, bool locked) 
{
    if (!enabled) return false;

    TouchEvent state;
    switch (touch.state)
    {
    case TouchEvent::START:
        this->recognized = false;
        this->success = true;
        this->startX = touch.x;
        this->startY = touch.y;
        this->x = touch.x;
        this->y = touch.y;
        break;
    case TouchEvent::STAY:
    case TouchEvent::END:
        deltaX = touch.x - this->x;
        deltaY = touch.y - this->y;

        this->x = touch.x;
        this->y = touch.y;

        state = touch.state;

        if (!this->recognized && !locked && (deltaX != 0 || deltaY != 0)) 
        {
            switch (axis)
            {
            case PanAxis::HORIZONTAL:
                success = fabs(deltaX) > fabs(deltaY);
                break;
            case PanAxis::VERTICAL:
                success = fabs(deltaX) < fabs(deltaY);
                break;
            case PanAxis::NONE:
                success = true;
                break;
            }
            this->recognized = true;
            state = TouchEvent::START;
        }

        if (respond && recognized && success && !locked) 
        {
            PanGestureCtx result
            {
                .state = state,
                .startX = this->startX,
                .startY = this->startY,
                .currentX = touch.x,
                .currentY = touch.y,
                .deltaX = deltaX,
                .deltaY = deltaY
            };
            this->respond(result);
        }
        break;
    case TouchEvent::NONE:
        this->success = false;
        break;
    }

    return this->success;
}

};