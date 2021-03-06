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

// Contains info about acceleration on pan ends
struct PanAcceleration
{
    // distances in pixels
    float distanceX;
    float distanceY;

    // times to cover the distance
    float timeX;
    float timeY;
};

// Current status of gesture
struct PanGestureStatus
{
    GestureState state; // Gesture state
    float x; // Current X position
    float y; // Current Y position
    float startX; // Start X position
    float startY; // Start Y position
    float deltaX; // Difference between current and previous positions by X
    float deltaY; // Difference between current and previous positions by Y
    
    // Acceleration info, NOT NULL ONLY from
    // gesture callback and when current state is END
    PanAcceleration acceleration;
};

typedef std::function<void(PanGestureStatus)> PanGestureRespond;

// Axis of pan recognition start conditions
enum class PanAxis
{
    HORIZONTAL, // Triggers only on horizontal coordinate changes
    VERTICAL, // Triggers only on vertical coordinate changes
    ANY, // Any movement allowed
};

struct PanPosition
{
    float x;
    float y;
};

// Pan recognizer
// UNSURE: while touch not moved enough to recognize it as pan
// START: gesture has been recognized
// MOVE: gesture in process
// END: finger released, acceleration will be calculated
// FAILED: unsupported
class PanGestureRecognizer : public GestureRecognizer
{
  public:
    PanGestureRecognizer(PanGestureRespond respond, PanAxis axis);
    GestureState recognitionLoop(TouchState touch, View* view, bool* shouldPlayDefaultSound) override;

    // Get pan gesture axis
    PanAxis getAxis() const { return this->axis; }
    
    // Get current state of recognizer
    PanGestureStatus getCurrentStatus();

  private:
    PanGestureRespond respond;
    float x;
    float y;
    float startX;
    float startY;
    float deltaX;
    float deltaY;
    PanAxis axis;
    std::vector<PanPosition> posHistory;
    GestureState lastState;
};

};
