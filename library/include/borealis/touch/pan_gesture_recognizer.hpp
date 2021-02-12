/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala
    Copyright (C) 2021  XITRIX

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "gesture_recognizer.hpp"

#define MAX_DELTA_MOVEMENT 10

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

class PanGestureRecognizer: public GestureRecognizer
{
public:
	PanGestureRecognizer(PanGestureRespond respond, PanAxis axis);
	GestureState recognitionLoop(TouchState touch, View* view) override;
	
	double getX() const { return x; }
	double getY() const { return y; }
	double getStartX() const { return startX; }
	double getStartY() const { return startY; }
	double getDeltaX() const { return deltaX; }
	double getDeltaY() const { return deltaY; }
	PanAxis getAxis() const { return this->axis; }
private:
	PanGestureRespond respond;
	double x;
	double y;
	double startX;
	double startY;
	double deltaX;
	double deltaY;
	PanAxis axis;
};

};