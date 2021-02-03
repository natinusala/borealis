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

namespace brls
{

struct PanGestureCtx;
typedef std::function<void(PanGestureCtx)> PanGestureRespond;

enum class PanAxis
{
    HORIZONTAL,
    VERTICAL,
	NONE,
};

struct PanGestureCtx
{
	TouchClickState state;
	double startX;
	double startY;
	double currentX;
	double currentY;
	double deltaX;
	double deltaY;
};

class PanGestureRecognizer: public GestureRecognizer
{
public:
	PanGestureRecognizer(PanGestureRespond respond, PanAxis axis);
	bool recognitionLoop(TouchState touch, bool locked) override;
	
	PanAxis getAxis() const
	{
		return this->axis;
	}
private:
	PanGestureRespond respond;
	PanAxis axis;
	double x;
	double y;
	double startX;
	double startY;
	double deltaX;
	double deltaY;
	bool recognized;
};

};