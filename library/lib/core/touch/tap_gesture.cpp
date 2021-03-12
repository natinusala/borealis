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
{
}

GestureState TapGestureRecognizer::recognitionLoop(Touch touch, View* view, bool* shouldPlayDefaultSound)
{
    if (!enabled)
        return GestureState::FAILED;

    // If not first touch frame and state is
    // INTERRUPTED or FAILED, stop recognition
    if (touch.phase != TouchPhase::START)
    {
        if (this->state == GestureState::INTERRUPTED || this->state == GestureState::FAILED)
        {
            if (respond && !this->callbackOnEndOnly && this->state != lastState)
                this->respond(getCurrentStatus());

            lastState = this->state;
            return this->state;
        }
    }

    switch (touch.phase)
    {
        case TouchPhase::START:
            this->state = GestureState::UNSURE;
            this->position = touch.position;

            if (respond && !this->callbackOnEndOnly)
                *shouldPlayDefaultSound &= this->respond(getCurrentStatus());
            break;
        case TouchPhase::STAY:
            // Check if touch is out view's bounds
            // if true, FAIL recognition
            if (touch.position.x < view->getX() || touch.position.x > view->getX() + view->getWidth() || touch.position.y < view->getY() || touch.position.y > view->getY() + view->getHeight())
            {
                this->state = GestureState::FAILED;
                if (respond && !this->callbackOnEndOnly)
                    this->respond(getCurrentStatus());
            }
            break;
        case TouchPhase::END:
            this->state = GestureState::END;
            if (respond)
                this->respond(getCurrentStatus());
            break;
        case TouchPhase::NONE:
            this->state = GestureState::FAILED;
            break;
    }

    lastState = this->state;
    return this->state;
}

TapGestureStatus TapGestureRecognizer::getCurrentStatus()
{
    return TapGestureStatus
    {
        .state = this->state,
        .position = this->position,
    };
}

};
