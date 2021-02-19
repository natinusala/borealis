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

#pragma once

#include <functional>
#include <string>

namespace brls
{

class Swkbd
{
  public:
    static bool openForText(std::function<void(std::string)> f, std::string headerText = "", std::string subText = "", int maxStringLength = 32, std::string initialText = "");
    static bool openForNumber(std::function<void(int)> f, std::string headerText = "", std::string subText = "", int maxStringLength = 32, std::string initialText = "", std::string leftButton = "", std::string rightButton = "");
};

} // namespace brls
