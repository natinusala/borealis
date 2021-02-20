/*
    Copyright 2021 natinusala

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

#include <borealis/core/application.hpp>
#include <borealis/platforms/switch/switch_input.hpp>

namespace brls
{

static const uint64_t HidNpadButton_None = ((uint64_t)1 << (63));

static const uint64_t SWITCH_BUTTONS_MAPPING[_BUTTON_MAX] = {
    HidNpadButton_ZL, // BUTTON_LT
    HidNpadButton_L, // BUTTON_LB

    HidNpadButton_StickL, // BUTTON_LSB

    // TODO: stop using the Any bitmasks after implementing axes to dpad in brls
    HidNpadButton_AnyUp, // BUTTON_UP
    HidNpadButton_AnyRight, // BUTTON_RIGHT
    HidNpadButton_AnyDown, // BUTTON_DOWN
    HidNpadButton_AnyLeft, // BUTTON_LEFT

    HidNpadButton_Minus, // BUTTON_BACK
    HidNpadButton_None, // BUTTON_GUIDE
    HidNpadButton_Plus, // BUTTON_START

    HidNpadButton_StickR, // BUTTON_RSB

    HidNpadButton_Y, // BUTTON_Y
    HidNpadButton_B, // BUTTON_B
    HidNpadButton_A, // BUTTON_A
    HidNpadButton_X, // BUTTON_X

    HidNpadButton_R, // BUTTON_RB
    HidNpadButton_ZR, // BUTTON_RT
};

SwitchInputManager::SwitchInputManager()
{
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&this->padState);
}

void SwitchInputManager::updateControllerState(ControllerState* state)
{
    padUpdate(&this->padState);
    uint64_t keysDown = padGetButtons(&this->padState);

    for (size_t i = 0; i < _BUTTON_MAX; i++)
    {
        uint64_t switchKey = SWITCH_BUTTONS_MAPPING[i];
        state->buttons[i]  = keysDown & switchKey;
    }
}

void SwitchInputManager::updateTouchState(TouchState* state)
{
    // Get gamepad state
    float x = oldTouch.x;
    float y = oldTouch.y;
    
    HidTouchScreenState hidState={0};
    if (hidGetTouchScreenStates(&hidState, 1)) 
    {
        if (hidState.count > 0) {
            x = hidState.touches[0].x / Application::windowScale;
            y = hidState.touches[0].y / Application::windowScale;
        }
    }

    state->x = x;
    state->y = y;

    if (hidState.count > 0) {
        if (oldTouch.state == TouchEvent::START || oldTouch.state == TouchEvent::STAY) {
            state->state = TouchEvent::STAY;
        } else {
            state->state = TouchEvent::START;
        }
    } else {
        if (oldTouch.state == TouchEvent::END || oldTouch.state == TouchEvent::NONE) {
            state->state = TouchEvent::NONE;
        } else {
            state->state = TouchEvent::END;
        }
    }
    oldTouch = *state;
}

} // namespace brls
