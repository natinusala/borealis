/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala

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

} // namespace brls
