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

// TODO: get real FPS
// Uses to calculate time to play acceleration animation
#define FPS 60.0f 

// Delta from touch starting point to current, when 
// touch will be recognized as pan movement
#define MAX_DELTA_MOVEMENT 10

// Touch history limit which uses to calculate current pan speed
#define HISTORY_LIMIT 2

// Negative acceleration to calculate 
// time to play acceleration animation
#define PAN_SCROLL_ACCELERATION -5000

namespace brls
{

PanGestureRecognizer::PanGestureRecognizer(PanGestureEvent::Callback respond, PanAxis axis)
    : axis(axis)
{
    panEvent.subscribe(respond);
}

GestureState PanGestureRecognizer::recognitionLoop(TouchState touch, View* view, Sound* soundToPlay)
{
    if (!enabled)
        return GestureState::FAILED;

    // If not first touch frame and state is
    // INTERRUPTED or FAILED, stop recognition
    if (touch.phase != TouchPhase::START)
    {
        if (this->state == GestureState::INTERRUPTED || this->state == GestureState::FAILED)
        {
            if (this->state != lastState)
                this->panEvent.fire(getCurrentStatus());

            lastState = this->state;
            return this->state;
        }
    }

    switch (touch.phase)
    {
        case TouchPhase::START:
            this->posHistory.clear();
            this->state         = GestureState::UNSURE;
            this->startPosition = touch.position;
            this->position      = touch.position;
            break;
        case TouchPhase::STAY:
        case TouchPhase::END:

            this->delta = touch.position - this->position;

            this->position = touch.position;

            // Check if pass any condition to set state START
            if (this->state == GestureState::UNSURE)
            {
                if (fabs(this->startPosition.x - touch.position.x) > MAX_DELTA_MOVEMENT || fabs(this->startPosition.y - touch.position.y) > MAX_DELTA_MOVEMENT)
                {
                    switch (axis)
                    {
                        case PanAxis::HORIZONTAL:
                            if (fabs(delta.x) > fabs(delta.y))
                                this->state = GestureState::START;
                            break;
                        case PanAxis::VERTICAL:
                            if (fabs(delta.x) < fabs(delta.y))
                                this->state = GestureState::START;
                            break;
                        case PanAxis::ANY:
                            this->state = GestureState::START;
                            break;
                    }
                }
            }
            else
            {
                if (touch.phase == TouchPhase::STAY)
                    this->state = GestureState::STAY;
                else
                    this->state = GestureState::END;
            }

            // If last touch frame, calculate acceleration

            static PanAcceleration acceleration;
            if (this->state == GestureState::END)
            {
                float time = posHistory.size() / FPS;

                float distanceX = posHistory[posHistory.size()].x - posHistory[0].x;
                float distanceY = posHistory[posHistory.size()].y - posHistory[0].y;

                float velocityX = distanceX / time;
                float velocityY = distanceY / time;

                acceleration.time.x = -fabs(velocityX) / PAN_SCROLL_ACCELERATION;
                acceleration.time.y = -fabs(velocityY) / PAN_SCROLL_ACCELERATION;

                acceleration.distance.x = velocityX * acceleration.time.x / 2;
                acceleration.distance.y = velocityY * acceleration.time.y / 2;
            }

            if (this->state == GestureState::START || this->state == GestureState::STAY || this->state == GestureState::END)
            {
                PanGestureStatus state = getCurrentStatus();
                state.acceleration     = acceleration;
                this->panEvent.fire(state);
            }

            break;
        case TouchPhase::NONE:
            this->state = GestureState::FAILED;
            break;
    }

    // Add current state to history
    posHistory.insert(posHistory.begin(), this->position);
    while (posHistory.size() > HISTORY_LIMIT)
    {
        posHistory.pop_back();
    }

    return this->state;
}

PanGestureStatus PanGestureRecognizer::getCurrentStatus()
{
    return PanGestureStatus {
        .state         = this->state,
        .position      = this->position,
        .startPosition = this->startPosition,
        .delta         = this->delta,
    };
}

};
