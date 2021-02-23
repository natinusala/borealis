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

#include <functional>

#include <borealis/core/input.hpp>

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
    END, // User released it's finger from screen, final frame of gesture
    FAILED, // Gesture failed conditions
};

// Superclass for all the other recognizers
class GestureRecognizer
{
public:
    // Main recognition loop, for internal usage only, should not be called anywhere, but Application
    virtual GestureState recognitionLoop(TouchState touch, View* view);
    
    // Returns true if recognizer plays default 'click' sound on touch start 
    virtual bool soundOnTouch();
    
    // Interupt this recognizer
    // If onlyIfUnsureState == true recognizer will be interupted
    // only if current state is UNSURE
    void interrupt(bool onlyIfUnsureState);
    
    // If false, this recognizer will be skipped
    bool enabled = true;
    
    // Get the current state of recognizer
	GestureState getState() const { return state; }
protected:
	GestureState state;
};

};
