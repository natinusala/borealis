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

#include <borealis/core/input.hpp>

namespace brls
{

TouchState InputManager::computeTouchState(RawTouchState currentTouch, TouchState lastFrameState)
{
    if (currentTouch.pressed)
    {
        lastFrameState.position = currentTouch.position;
        if (lastFrameState.phase == TouchPhase::START || lastFrameState.phase == TouchPhase::STAY)
            lastFrameState.phase = TouchPhase::STAY;
        else
            lastFrameState.phase = TouchPhase::START;
    }
    else
    {
        if (lastFrameState.phase == TouchPhase::END || lastFrameState.phase == TouchPhase::NONE)
            lastFrameState.phase = TouchPhase::NONE;
        else
            lastFrameState.phase = TouchPhase::END;
    }

    if (currentTouch.scroll.x != 0 || currentTouch.scroll.y != 0)
    {
        lastFrameState.position = currentTouch.position;
    }
    lastFrameState.scroll = currentTouch.scroll;

    return lastFrameState;
}

} // namespace brls
