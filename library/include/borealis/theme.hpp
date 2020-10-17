/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
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

namespace brls
{

enum class ThemeVariant
{
    LIGHT,
    DARK
};

// A Theme instance contains colors for one variant (light or dark)
class Theme
{
  public:
    float backgroundColor[3]; // gl color
    NVGcolor backgroundColorRGB;

    NVGcolor textColor;
    NVGcolor descriptionColor;

    NVGcolor notificationTextColor;
    NVGcolor backdropColor;

    NVGcolor separatorColor;

    NVGcolor sidebarColor;
    NVGcolor activeTabColor;
    NVGcolor sidebarSeparatorColor;

    NVGcolor highlightBackgroundColor;
    NVGcolor highlightColor1;
    NVGcolor highlightColor2;

    NVGcolor listItemSeparatorColor;
    NVGcolor listItemValueColor;
    NVGcolor listItemFaintValueColor;

    NVGcolor tableEvenBackgroundColor;
    NVGcolor tableBodyTextColor;

    NVGcolor dropdownBackgroundColor;

    NVGcolor nextStageBulletColor;

    NVGcolor spinnerBarColor;

    NVGcolor headerRectangleColor;

    NVGcolor buttonPrimaryEnabledBackgroundColor;
    NVGcolor buttonPrimaryDisabledBackgroundColor;
    NVGcolor buttonPrimaryEnabledTextColor;
    NVGcolor buttonPrimaryDisabledTextColor;
    NVGcolor buttonBorderedBorderColor;
    NVGcolor buttonBorderedTextColor;
    NVGcolor buttonRegularBackgroundColor;
    NVGcolor buttonRegularTextColor;
    NVGcolor buttonRegularBorderColor;

    NVGcolor dialogColor;
    NVGcolor dialogBackdrop;
    NVGcolor dialogButtonColor;
    NVGcolor dialogButtonSeparatorColor;
};

// Helper class to store two Theme variants and get the right one
// depending on current system theme
template <class LightTheme, class DarkTheme>
class GenericThemeVariantsWrapper
{
  private:
    LightTheme* lightTheme;
    DarkTheme* darkTheme;

  public:
    GenericThemeVariantsWrapper(LightTheme* lightTheme, DarkTheme* darkTheme)
        : lightTheme(lightTheme)
        , darkTheme(darkTheme)
    {
    }

    Theme* getTheme(ThemeVariant currentThemeVariant)
    {
        if (currentThemeVariant == ThemeVariant::DARK)
            return this->darkTheme;

        return this->lightTheme;
    }

    Theme* getLightTheme()
    {
        return this->lightTheme;
    }

    Theme* getDarkTheme()
    {
        return this->darkTheme;
    }

    ~GenericThemeVariantsWrapper()
    {
        delete this->lightTheme;
        delete this->darkTheme;
    }
};

// Themes variants wrapper specification for built-in library views
typedef GenericThemeVariantsWrapper<Theme, Theme> LibraryViewsThemeVariantsWrapper;

class HorizonLightTheme : public Theme
{
  public:
    HorizonLightTheme();
};

class HorizonDarkTheme : public Theme
{
  public:
    HorizonDarkTheme();
};

} // namespace brls
