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

#pragma once

#include <borealis/core/gesture.hpp>

namespace brls
{

struct TapGestureConfig
{
    bool highlightOnSelect = true;
    Sound unsureSound = SOUND_FOCUS_CHANGE;
    Sound failedSound = SOUND_TOUCH_UNFOCUS;
    Sound endSound = SOUND_CLICK;

    TapGestureConfig() {}
    TapGestureConfig(bool highlightOnSelect, Sound unsureSound, Sound failedSound,Sound endSound) 
    {
        this->highlightOnSelect = highlightOnSelect;
        this->unsureSound = unsureSound;
        this->failedSound = failedSound;
        this->endSound = endSound;
    }
};

struct TapGestureStatus
{
    GestureState state; // Gesture state
    Point position; // Current position
};
typedef std::function<void(TapGestureStatus, Sound*)> TapGestureRespond;

// Tap recognizer
// UNSURE: while touch moves inside of View bounds
// START: unsupported
// MOVE: unsupported
// END: touch released inside View's bounds
// FAILED: touch moved outside View's bounds
class TapGestureRecognizer : public GestureRecognizer
{
  public:
    // Simple ctor which uses View's primary action as response which will be called only on recognizer state END.
    TapGestureRecognizer(View* view, TapGestureConfig config = TapGestureConfig());

    // Simple ctor with custom response which will be called only on recognizer state END.
    TapGestureRecognizer(View* view, std::function<void()> respond, TapGestureConfig config = TapGestureConfig());

    // Complex ctor with fully controllable response.
    TapGestureRecognizer(TapGestureRespond respond);
    
    GestureState recognitionLoop(TouchState touch, View* view, Sound* soundToPlay) override;

    // Get current state of recognizer
    TapGestureStatus getCurrentStatus();

  private:
    TapGestureRespond respond;
    Point position;
    GestureState lastState;
};

};
