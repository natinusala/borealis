/*
    Copyright 2019-2020 natinusala
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

#include <initializer_list>
#include <string>
#include <unordered_map>

namespace brls
{

struct Theme;

// Wrapper to grab metrics from a theme
// KEPT FOR BACKWARDS COMPATIBILITY
struct Style
{
    Style(Theme theme);
    
    // Shortcut for Theme.getMetric(name)
    float operator[](const std::string name);

    private:
    Theme *parentTheme;
};

} // namespace brls
