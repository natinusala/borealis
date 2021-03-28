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

#include <borealis/core/touch/pan_gesture.hpp>

namespace brls
{

/*
 * Scroll recognizer
 *
 * Child of Pan recognizer.
 * The only difference is that Scroll recognizer will translate scrolling wheel to pan gestures.
 * If NO_TOUCH_SCROLLING=true defined, will ignore scrolling by touch.
 * 
 * If mouse translation used, the only available state is MOVE.
 * Also PanGestureStatus will be returned with delta values ONLY.
 */
class ScrollGestureRecognizer : public PanGestureRecognizer
{
  public:
    ScrollGestureRecognizer(PanGestureEvent::Callback respond, PanAxis axis);
    GestureState recognitionLoop(TouchState touch, View* view, Sound* soundToPlay) override;
};

};
