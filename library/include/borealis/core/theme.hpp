/*
    Copyright 2019-2020 natinusala
    Copyright 2019 p-sam
    Copyright 2021 EmreTech

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
#include <tinyxml2.h>

#include <string>
#include <unordered_map>

namespace brls
{

enum class ThemeVariant
{
    LIGHT,
    DARK
};

struct Theme
{
    Theme(std::string name);

    void inflateFromXMLElement(tinyxml2::XMLElement *element);
    void inflateFromXMLString(const std::string xml);
    void inflateFromXMLFile(const std::string path);

    float getMetric(const std::string path, ThemeVariant variant);
    NVGcolor getColor(const std::string path, ThemeVariant variant);

    private:
    // Each color/metric has a key of their theme variant + prefix
    // An example: "dark/brls/sidebar/background"

    std::unordered_map<std::string, NVGcolor> colors;
    std::unordered_map<std::string, float> metrics;

    std::string name;
};

} // namespace brls
