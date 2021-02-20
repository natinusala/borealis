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

#include "gesture_recognizer.hpp"

namespace brls
{

class PanGestureRecognizer;
typedef std::function<void(PanGestureRecognizer*)> PanGestureRespond;

enum class PanAxis
{
    HORIZONTAL,
    VERTICAL,
	NONE,
};

struct position
{
    float x;
    float y;
};

struct pan_acceleration
{
    float distanceX;
    float distanceY;
    float timeX;
    float timeY;
};

class PanGestureRecognizer: public GestureRecognizer
{
public:
	PanGestureRecognizer(PanGestureRespond respond, PanAxis axis);
	GestureState recognitionLoop(TouchState touch, View* view) override;
	
	float getX() const { return x; }
	float getY() const { return y; }
	float getStartX() const { return startX; }
	float getStartY() const { return startY; }
	float getDeltaX() const { return deltaX; }
	float getDeltaY() const { return deltaY; }
	PanAxis getAxis() const { return this->axis; }
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
};

};
