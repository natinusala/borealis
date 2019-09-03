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

#include <borealis/box_layout.hpp>
#include <borealis/image.hpp>
#include <borealis/label.hpp>
#include <borealis/rectangle.hpp>
#include <string>

namespace brls
{

// A list item
// TODO: Use a Label with integrated ticker
class ListItem : public View
{
  private:
    std::string label;
    std::string subLabel;
    std::string value;
    bool valueFaint;

    std::string oldValue;
    bool oldValueFaint;
    float valueAnimation = 0.0f;

    bool checked = false; // check mark on the right

    unsigned textSize;

    bool drawTopSeparator = true;

    Label* descriptionView = nullptr;
    Image* thumbnailView   = nullptr;

    EventListener clickListener = nullptr;

    bool indented = false;

    void resetValueAnimation();

  public:
    ListItem(std::string label, std::string description = "", std::string subLabel = "");

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    View* requestFocus(FocusDirection direction, View* oldFocus, bool fromUp) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    void getHighlightInsets(unsigned* top, unsigned* right, unsigned* bottom, unsigned* left) override;
    void setParent(View* parent) override;
    bool onClick() override;

    void setThumbnail(std::string imagePath);
    void setThumbnail(unsigned char* buffer, size_t bufferSize);

    bool hasDescription();
    void setDrawTopSeparator(bool draw);

    void setIndented(bool indented);

    void setTextSize(unsigned textSize);

    void setChecked(bool checked);

    std::string getLabel();

    /**
     * Sets the value of this list item
     * (the text on the right)
     * Set faint to true to have the new value
     * use a darker color (typically "OFF" labels)
     */
    void setValue(std::string value, bool faint = false, bool animate = true);
    std::string getValue();

    void setClickListener(EventListener listener);

    ~ListItem();
};

// Some spacing (to make groups of ListItems)
class ListItemGroupSpacing : public Rectangle
{
  public:
    ListItemGroupSpacing(bool separator = false);
};

// A list item with mutliple choices for its value
// (will open a Dropdown)
typedef std::function<void(int result)> SelectListener;

class SelectListItem : public ListItem
{
  public:
    SelectListItem(std::string label, std::vector<std::string> values, unsigned selectedValue = 0);

    void setListener(SelectListener listener);
    void setSelectedValue(unsigned value);

  private:
    std::vector<std::string> values;
    unsigned selectedValue;

    SelectListener listener = [](unsigned result) {};
};

// A list item with a ON/OFF value
// that can be toggled
// Use the clickListener to detect when the value
// changes
class ToggleListItem : public ListItem
{
  private:
    bool toggleState;
    std::string onValue, offValue;

    void updateValue();

  public:
    ToggleListItem(std::string label, bool initialValue, std::string description = "", std::string onValue = "On", std::string offValue = "Off");

    bool onClick() override;

    bool getToggleState();
};

// A list item which spawns the swkbd
// to input its value (string)
class InputListItem : public ListItem
{
  protected:
    std::string helpText;
    int maxInputLength;

  public:
    InputListItem(std::string label, std::string initialValue, std::string helpText, std::string description = "", int maxInputLength = 32);

    bool onClick() override;
};

// A list item which spawns the swkbd
// to input its value (integer)
class IntegerInputListItem : public InputListItem
{
  public:
    IntegerInputListItem(std::string label, int initialValue, std::string helpText, std::string description = "", int maxInputLength = 32);

    bool onClick() override;
};

// A vertical list of various widgets, with proper margins and spacing
// and a scroll bar
class List : public BoxLayout
{
  public:
    List(size_t defaultFocus = 0);

    View* defaultFocus(View* oldFocus) override;

  protected:
    void customSpacing(View* current, View* next, int* spacing) override;
};

} // namespace brls
