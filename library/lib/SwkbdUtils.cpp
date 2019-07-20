/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  WerWolv

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

#include "SwkbdUtils.hpp"

#include <cstring>

#ifdef __SWITCH__
#include <switch.h>
#endif

bool askForKeyboardInputString(function<void(string)> f, string headerText, string subText, int maxStringLength, string initialText) {
    #ifdef __SWITCH__

    SwkbdConfig config;

    swkbdCreate(&config, 0);

    swkbdConfigMakePresetDefault(&config);
    swkbdConfigSetHeaderText(&config, headerText.c_str());
    swkbdConfigSetSubText(&config, subText.c_str());
    swkbdConfigSetInitialText(&config, initialText.c_str());
    swkbdConfigSetBlurBackground(&config, true);
    swkbdConfigSetType(&config, SwkbdType_Normal);
    swkbdConfigSetStringLenMax(&config, maxStringLength);
    swkbdConfigSetStringLenMaxExt(&config, 1);
    swkbdConfigSetKeySetDisableBitmask(&config, SwkbdKeyDisableBitmask_At | SwkbdKeyDisableBitmask_Percent | SwkbdKeyDisableBitmask_ForwardSlash | SwkbdKeyDisableBitmask_Backslash);

    char buffer[0x100];

    if (R_SUCCEEDED(swkbdShow(&config, buffer, 0x100)) && strcmp(buffer, "") != 0) {
        f(buffer);

        swkbdClose(&config);
        return true;
    }

    swkbdClose(&config);

    return false;

    #else

    f("");
    return true;

    #endif
}

bool askForKeyboardInputInteger(function<void(int)> f, string headerText, string subText, int maxStringLength, string initialText, string leftButton, string rightButton) {
    #ifdef __SWITCH__

    SwkbdConfig config;

    swkbdCreate(&config, 0);

    swkbdConfigMakePresetDefault(&config);
    swkbdConfigSetHeaderText(&config, headerText.c_str());
    swkbdConfigSetSubText(&config, subText.c_str());
    swkbdConfigSetInitialText(&config, initialText.c_str());
    swkbdConfigSetBlurBackground(&config, true);
    swkbdConfigSetType(&config, SwkbdType_NumPad);
    swkbdConfigSetStringLenMax(&config, maxStringLength);
    swkbdConfigSetLeftOptionalSymbolKey(&config, leftButton.c_str());
    swkbdConfigSetRightOptionalSymbolKey(&config, rightButton.c_str());
    swkbdConfigSetStringLenMaxExt(&config, 1);
    swkbdConfigSetKeySetDisableBitmask(&config, SwkbdKeyDisableBitmask_At | SwkbdKeyDisableBitmask_Percent | SwkbdKeyDisableBitmask_ForwardSlash | SwkbdKeyDisableBitmask_Backslash);

    char buffer[0x100];

    if (R_SUCCEEDED(swkbdShow(&config, buffer, 0x100)) && strcmp(buffer, "") != 0) {
        f(stol(buffer));

        swkbdClose(&config);
        return true;
    }

    swkbdClose(&config);

    return false;

    #else
    
    f(0);
    return true;

    #endif
}