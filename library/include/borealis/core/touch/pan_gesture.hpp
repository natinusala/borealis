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

class PanGestureRecognizer;
typedef std::function<void(PanGestureRecognizer*)> PanGestureRespond;

// Axis of pan recognition start conditions
enum class PanAxis
{
    HORIZONTAL, // Triggers only on horizontal coordinate changes
    VERTICAL, // Triggers only on vertical coordinate changes
    NONE, // Any movement allowed
};

struct position
{
    float x;
    float y;
};

// Contains info about acceleration on pan ends
struct pan_acceleration
{
    // distances in pixels
    float distanceX;
    float distanceY;

    // times to cover the distance
    float timeX;
    float timeY;
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
    GestureState recognitionLoop(TouchState touch, View* view) override;

    // Get current X position
    float getX() const { return x; }

    // Get current Y position
    float getY() const { return y; }

    // Get start X position
    float getStartX() const { return startX; }

    // Get start Y position
    float getStartY() const { return startY; }

    // Get difference between current and previous positions by X
    float getDeltaX() const { return deltaX; }

    // Get difference between current and previous positions by Y
    float getDeltaY() const { return deltaY; }

    //
    PanAxis getAxis() const { return this->axis; }

    // Get acceleration info, actual data only when current state is END
    pan_acceleration getAcceleration() const { return this->acceleration; }

  private:
    PanGestureRespond respond;
    float x;
    float y;
    float startX;
    float startY;
    float deltaX;
    float deltaY;
    PanAxis axis;
    pan_acceleration acceleration;
    std::vector<position> posHistory;
    GestureState lastState;
};

};
