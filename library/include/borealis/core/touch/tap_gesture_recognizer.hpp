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

class TapGestureRecognizer;
typedef std::function<bool(TapGestureRecognizer*)> TapGestureRespond;

class TapGestureRecognizer: public GestureRecognizer
{
public:
	TapGestureRecognizer(TapGestureRespond respond, bool callbackOnEndOnly = true);
	GestureState recognitionLoop(TouchState touch, View* view) override;
    bool soundOnTouch() override;
private:
	TapGestureRespond respond;
    bool playSound;
	float x;
	float y;
    bool callbackOnEndOnly;
    GestureState lastState;
};

};
