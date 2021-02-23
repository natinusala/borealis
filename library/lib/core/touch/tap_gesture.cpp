/*
    Copyright 2021 XITRIX

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

#include <borealis.hpp>

namespace brls
{

TapGestureRecognizer::TapGestureRecognizer(TapGestureRespond respond, bool callbackOnEndOnly)
    : respond(respond)
    , callbackOnEndOnly(callbackOnEndOnly)
    , playSound(true)
{
}

GestureState TapGestureRecognizer::recognitionLoop(TouchState touch, View* view)
{
    if (!enabled)
        return GestureState::FAILED;

    // If not first touch frame and state is
    // INTERRUPTED or FAILED, stop recognition
    if (touch.state != TouchEvent::START)
    {
        if (this->state == GestureState::INTERRUPTED || this->state == GestureState::FAILED)
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
            this->x     = touch.x;
            this->y     = touch.y;

            if (respond && !this->callbackOnEndOnly)
                this->playSound = this->respond(this);
            break;
        case TouchEvent::STAY:
            // Check if touch is out view's bounds
            // if true, FAIL recognition
            if (touch.x < view->getX() || touch.x > view->getX() + view->getWidth() || touch.y < view->getY() || touch.y > view->getY() + view->getHeight())
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

bool TapGestureRecognizer::soundOnTouch()
{
    return this->playSound;
}

};
