/*
    Copyright 2019-2020 natinusala
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

#include <nanovg.h>

#include <initializer_list>
#include <string>
#include <unordered_map>

namespace brls
{

enum class ThemeVariant
{
    LIGHT,
    DARK
};

class ThemeValues
{
  public:
    ThemeValues(std::initializer_list<std::pair<std::string, NVGcolor>> list);

    void addColor(std::string name, NVGcolor color);
    NVGcolor getColor(std::string name);

  private:
    std::unordered_map<std::string, NVGcolor> values;
};

// Simple wrapper around ThemeValues for the array operator
class Theme
{
  public:
    Theme(ThemeValues* values);
    NVGcolor operator[](std::string name);

    void addColor(std::string name, NVGcolor color);
    NVGcolor getColor(std::string name);

  private:
    ThemeValues* values;
};

Theme getLightTheme();
Theme getDarkTheme();

} // namespace brls
