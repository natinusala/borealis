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

#define FPS 60.0f // TODO: get real FPS
#define MAX_DELTA_MOVEMENT 10
#define HISTORY_LIMIT 3
#define PAN_SCROLL_ACCELERATION -5000

namespace brls
{

PanGestureRecognizer::PanGestureRecognizer(PanGestureRespond respond, PanAxis axis)
    : respond(respond)
    , axis(axis)
{
}

GestureState PanGestureRecognizer::recognitionLoop(TouchState touch, View* view, bool* shouldPlayDefaultSound)
{
    if (!enabled)
        return GestureState::FAILED;

    // If not first touch frame and state is
    // INTERRUPTED or FAILED, stop recognition
    if (touch.state != TouchEvent::START)
    {
        if (this->state == GestureState::INTERRUPTED || this->state == GestureState::FAILED)
        {
            if (respond && this->state != lastState)
                this->respond(this);

            lastState = this->state;
            return this->state;
        }
    }

    switch (touch.state)
    {
        case TouchEvent::START:
            this->posHistory.clear();
            this->state  = GestureState::UNSURE;
            this->startX = touch.x;
            this->startY = touch.y;
            this->x      = touch.x;
            this->y      = touch.y;
            break;
        case TouchEvent::STAY:
        case TouchEvent::END:

            this->deltaX = touch.x - this->x;
            this->deltaY = touch.y - this->y;

            this->x = touch.x;
            this->y = touch.y;

            // Check if pass any condition to set state START
            if (this->state == GestureState::UNSURE)
            {
                if (fabs(this->startX - touch.x) > MAX_DELTA_MOVEMENT || fabs(this->startY - touch.y) > MAX_DELTA_MOVEMENT)
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

            // If last touch frame, calculate acceleration
            if (this->state == GestureState::END)
            {
                float time = posHistory.size() / FPS;

                float distanceX = posHistory[posHistory.size()].x - posHistory[0].x;
                float distanceY = posHistory[posHistory.size()].y - posHistory[0].y;

                float velocityX = distanceX / time;
                float velocityY = distanceY / time;

                acceleration.timeX = -fabs(velocityX) / PAN_SCROLL_ACCELERATION;
                acceleration.timeY = -fabs(velocityY) / PAN_SCROLL_ACCELERATION;

                acceleration.distanceX = velocityX * acceleration.timeX / 2;
                acceleration.distanceY = velocityY * acceleration.timeY / 2;
            }

            if (this->state == GestureState::START || this->state == GestureState::STAY || this->state == GestureState::END)
            {
                this->respond(this);
            }

            break;
        case TouchEvent::NONE:
            this->state = GestureState::FAILED;
            break;
    }

    // Add current state to history
    posHistory.insert(posHistory.begin(), position { .x = this->x, .y = this->y });
    while (posHistory.size() > HISTORY_LIMIT)
    {
        posHistory.pop_back();
    }

    return this->state;
}

};
