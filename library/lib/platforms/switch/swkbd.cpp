/*
    Copyright 2019 WerWolv
    Copyright 2019 p-sam

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

#include <borealis/core/logger.hpp>
#include <borealis/platforms/switch/swkbd.hpp>
#include <cstring>
#include <iostream>

#ifdef __SWITCH__
#include <switch.h>
#endif

namespace brls
{

#ifdef __SWITCH__
static SwkbdConfig createSwkbdBaseConfig(std::string headerText, std::string subText, int maxStringLength, std::string initialText)
{
    SwkbdConfig config;

    swkbdCreate(&config, 0);

    swkbdConfigMakePresetDefault(&config);
    swkbdConfigSetHeaderText(&config, headerText.c_str());
    swkbdConfigSetSubText(&config, subText.c_str());
    swkbdConfigSetStringLenMax(&config, maxStringLength);
    swkbdConfigSetInitialText(&config, initialText.c_str());
    swkbdConfigSetStringLenMin(&config, 1);
    swkbdConfigSetBlurBackground(&config, true);

    return config;
}
#else
static std::string terminalInput(std::string text)
{
    printf("\033[0;94m[INPUT] \033[0;36m%s\033[0m: ", text.c_str());
    std::string line;
    std::getline(std::cin, line);
    return line;
}
#endif

bool Swkbd::openForText(std::function<void(std::string)> f, std::string headerText, std::string subText, int maxStringLength, std::string initialText)
{
#ifdef __SWITCH__
    SwkbdConfig config = createSwkbdBaseConfig(headerText, subText, maxStringLength, initialText);

    swkbdConfigSetType(&config, SwkbdType_Normal);
    swkbdConfigSetKeySetDisableBitmask(&config, SwkbdKeyDisableBitmask_At | SwkbdKeyDisableBitmask_Percent | SwkbdKeyDisableBitmask_ForwardSlash | SwkbdKeyDisableBitmask_Backslash);

    char buffer[0x100];

    if (R_SUCCEEDED(swkbdShow(&config, buffer, 0x100)) && strcmp(buffer, "") != 0)
    {
        f(buffer);

        swkbdClose(&config);
        return true;
    }

    swkbdClose(&config);

    return false;
#else
    std::string line = terminalInput(headerText);
    f(line);
    return true;
#endif
}

bool Swkbd::openForNumber(std::function<void(int)> f, std::string headerText, std::string subText, int maxStringLength, std::string initialText, std::string leftButton, std::string rightButton)
{
#ifdef __SWITCH__
    SwkbdConfig config = createSwkbdBaseConfig(headerText, subText, maxStringLength, initialText);

    swkbdConfigSetType(&config, SwkbdType_NumPad);
    swkbdConfigSetLeftOptionalSymbolKey(&config, leftButton.c_str());
    swkbdConfigSetRightOptionalSymbolKey(&config, rightButton.c_str());
    swkbdConfigSetKeySetDisableBitmask(&config, SwkbdKeyDisableBitmask_At | SwkbdKeyDisableBitmask_Percent | SwkbdKeyDisableBitmask_ForwardSlash | SwkbdKeyDisableBitmask_Backslash);

    char buffer[0x100];

    if (R_SUCCEEDED(swkbdShow(&config, buffer, 0x100)) && strcmp(buffer, "") != 0)
    {
        f(std::stol(buffer));

        swkbdClose(&config);
        return true;
    }

    swkbdClose(&config);

    return false;
#else
    std::string line = terminalInput(headerText);

    try
    {
        f(stol(line));
        return true;
    }
    catch (const std::exception& e)
    {
        Logger::error("Could not parse input, did you enter a valid integer?");
        return false;
    }
#endif
}

} // namespace brls
