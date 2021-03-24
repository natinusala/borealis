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

#include <borealis/core/audio.hpp>
#include <borealis/core/input.hpp>
#include <functional>

namespace brls
{

class View;

// Represents current gesture state
enum class GestureState
{
    INTERRUPTED, // Gesture has been interupted, no callbacks will come
    UNSURE, // Gesture started recognition and not sure if it should interupt other recognizers
    START, // Gesture sure that it match to conditions and will interupt other recognizers
    STAY, // Gesture in process, user still hold finger on screen
    END, // User released their finger from screen, final frame of gesture
    FAILED, // Gesture failed conditions
};

/*
* Superclass for all recognizers
* 
* To create a new type of gesture recognizer, you should implement 
* recognitionLoop method.
* 
* It should contain logic with changing gesture's state. 
* Recognizers' first state is UNSURE, in that state calling stack cannot tell
* which gesture user tries to apply. I.E. user puts and holds finger on the screen, so it can't be 
* told whether it's going to be a tap or a swipe
*
* If gesture has been recognized, change its state to START, but ONLY for the first frame, 
* on the next frame it should be changed to STAY and remain the same until the end, then it 
* should change state to END. 
* 
* When any recognizer changes its state to START, it sends an interrupt event to every recognizer in the stack,
* so don't forget to handle that case.
* 
* If gesture does not apply to recognizer's pattern, change its state to FAILED. 
* It could also be used as a placerholder when recognizer is not in use.
* 
* Use touch argument to get current state of touch.
*
* View argument contains the view to which this recognizer is attached.
* 
* Use soundToPlay pointer to set sound which will be played in current frame.
* Leave it empty or use SOUND_NONE to not play any sound.
*/
class GestureRecognizer
{
  public:
    virtual ~GestureRecognizer() { }

    // Main recognition loop, for internal usage only, should not be called anywhere, but Application
    virtual GestureState recognitionLoop(TouchState touch, View* view, Sound* soundToPlay);

    // Interrupt this recognizer
    // If onlyIfUnsureState == true recognizer will be interupted
    // only if current state is UNSURE
    void interrupt(bool onlyIfUnsureState);

    // If false, this recognizer will be skipped
    bool isEnabled() const { return this->enabled; }

    // If false, this recognizer will be skipped
    void setEnabled(bool enabled) { this->enabled = enabled; }

    // Get the current state of recognizer
    GestureState getState() const { return state; }

  protected:
    GestureState state = GestureState::FAILED;
    bool enabled       = true;
};

} // namespace brls
