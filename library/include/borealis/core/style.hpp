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

class StyleValues
{
  public:
    StyleValues(std::initializer_list<std::pair<std::string, float>> list);

    void addMetric(std::string name, float value);
    float getMetric(std::string name);

  private:
    std::unordered_map<std::string, float> values;
};

// Simple wrapper around StyleValues for the array operator
class Style
{
  public:
    Style(StyleValues* values);
    float operator[](std::string name);

    void addMetric(std::string name, float value);
    float getMetric(std::string name);

  private:
    StyleValues* values;
};

Style getStyle();

} // namespace brls
