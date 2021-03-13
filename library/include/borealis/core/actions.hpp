/*
    Copyright 2020 WerWolv

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

#include <borealis/core/audio.hpp>
#include <borealis/core/input.hpp>
#include <functional>
#include <string>

namespace brls
{

class View;

typedef std::function<bool(View*)> ActionListener;

typedef int ActionIdentifier;

#define ACTION_NONE -1

struct Action
{
    enum ControllerButton button;

    ActionIdentifier identifier;
    std::string hintText;
    bool available;
    bool hidden;
    enum Sound sound;
    ActionListener actionListener;

    bool operator==(const enum ControllerButton other)
    {
        return this->button == other;
    }
};

} // namespace brls
