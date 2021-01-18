/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
    Copyright (C) 2019  p-sam

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

#pragma once

#include <nanovg/nanovg.h>

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
