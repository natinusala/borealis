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

class TapGestureRecognizer;
typedef std::function<void(TapGestureRecognizer*)> TapGestureRespond;

class TapGestureRecognizer: public GestureRecognizer
{
public:
	TapGestureRecognizer(TapGestureRespond respond, bool callbackOnEndOnly = true);
	GestureState recognitionLoop(TouchState touch, View* view) override;
private:
	TapGestureRespond respond;
	float x;
	float y;
    bool callbackOnEndOnly;
    GestureState lastState;
};

};