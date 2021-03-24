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

TapGestureRecognizer::TapGestureRecognizer(View* view, TapGestureConfig config)
{
    this->tapEvent.subscribe([view, config](TapGestureStatus status, Sound* soundToPlay) {
        Application::giveFocus(view);
        for (auto& action : view->getActions())
        {
            if (action.button != static_cast<enum ControllerButton>(BUTTON_A))
                continue;

            if (action.available)
            {
                if (config.highlightOnSelect)
                    view->playClickAnimation(status.state != GestureState::UNSURE);

                switch (status.state)
                {
                    case GestureState::UNSURE:
                        *soundToPlay = config.unsureSound;
                        break;
                    case GestureState::FAILED:
                    case GestureState::INTERRUPTED:
                        *soundToPlay = config.failedSound;
                        break;
                    case GestureState::END:
                        if (action.actionListener(view))
                            *soundToPlay = action.sound;
                        break;
                }
            }
        }
    });
}

TapGestureRecognizer::TapGestureRecognizer(View* view, std::function<void()> respond, TapGestureConfig config)
{
    this->tapEvent.subscribe([view, respond, config](TapGestureStatus status, Sound* soundToPlay) {
        Application::giveFocus(view);
        if (config.highlightOnSelect)
            view->playClickAnimation(status.state != GestureState::UNSURE);

        switch (status.state)
        {
            case GestureState::UNSURE:
                *soundToPlay = config.unsureSound;
                break;
            case GestureState::FAILED:
            case GestureState::INTERRUPTED:
                *soundToPlay = config.failedSound;
                break;
            case GestureState::END:
                *soundToPlay = config.endSound;
                respond();
                break;
        }
    });
}

TapGestureRecognizer::TapGestureRecognizer(TapGestureEvent::Callback respond)
{
    tapEvent.subscribe(respond);
}

GestureState TapGestureRecognizer::recognitionLoop(TouchState touch, View* view, Sound* soundToPlay)
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
                this->tapEvent.fire(getCurrentStatus(), soundToPlay);

            lastState = this->state;
            return this->state;
        }
    }

    switch (touch.phase)
    {
        case TouchPhase::START:
            this->state    = GestureState::UNSURE;
            this->position = touch.position;
            this->tapEvent.fire(getCurrentStatus(), soundToPlay);
            break;
        case TouchPhase::STAY:
            // Check if touch is out view's bounds
            // if true, FAIL recognition
            if (touch.position.x < view->getX() || touch.position.x > view->getX() + view->getWidth() || touch.position.y < view->getY() || touch.position.y > view->getY() + view->getHeight())
            {
                this->state = GestureState::FAILED;
                this->tapEvent.fire(getCurrentStatus(), soundToPlay);
            }
            break;
        case TouchPhase::END:
            this->state = GestureState::END;
            this->tapEvent.fire(getCurrentStatus(), soundToPlay);
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
    return TapGestureStatus {
        .state    = this->state,
        .position = this->position,
    };
}

};
