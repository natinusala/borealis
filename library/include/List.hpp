/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

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

#include <string>

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Rectangle.hpp>

#define LIST_ITEM_VALUE_ANIMATION_DURATION 100

// A list item
// TODO: Use a Label with integrated ticker
class ListItem : public View
{
    private:
        string label;
        string value;
        bool valueFaint;

        string oldValue;
        bool oldValueFaint;
        float valueAnimation = 0.0f;

        bool selected = false; // check mark on the right

        unsigned textSize;

        bool drawTopSeparator = true;

        Label *sublabelView = nullptr;

        EventListener clickListener = nullptr;

        bool indented = false;

        void resetValueAnimation();

    public:
        ListItem(string label, string sublabel = "");

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;
        View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp) override;
        void layout(NVGcontext *vg, Style *style, FontStash *stash) override;
        void getHighlightInsets(unsigned *top, unsigned *right, unsigned *bottom, unsigned *left) override;
        void setParent(View *parent) override;
        bool onClick() override;

        bool hasSubLabel();
        void setDrawTopSeparator(bool draw);

        void setIndented(bool indented);

        void setTextSize(unsigned textSize);

        void setSelected(bool selected);

        string getLabel();

        /**
         * Sets the value of this list item
         * (the text on the right)
         * Set faint to true to have the new value
         * use a darker color (typically "OFF" labels)
         */
        void setValue(string value, bool faint = false, bool animate = true);
        string getValue();

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
// (will open a SelectView)
class SelectListItem : public ListItem
{
    public:
        SelectListItem(string label, vector<string> values, unsigned selectedValue = 0);

    private:
        vector<string> values;
        unsigned selectedValue;
};


// A list item with a ON/OFF value
// that can be toggled
// Use the clickListener to detect when the value
// changes
class ToggleListItem : public ListItem
{
    private:
        bool toggleState;
        string onValue, offValue;

        void updateValue();

    public:
        ToggleListItem(string label, bool initialValue, string sublabel = "", string onValue = "On", string offValue = "Off");

        bool onClick() override;

        bool getToggleState();
};

class InputListItem : public ListItem
{
    protected:
        string helpText;
        int maxInputLength;

    public:
        InputListItem(string label, string initialValue, string helpText, string sublabel = "", int maxInputLength = 32);

        bool onClick() override;
};

class IntegerInputListItem : public InputListItem
{
    public:
        IntegerInputListItem(string label, int initialValue, string helpText, string sublabel = "", int maxInputLength = 32);

        bool onClick() override;
};

// A vertical list of various widgets, with proper margins and spacing
// and a scroll bar
class List : public BoxLayout
{
    public:
        List();

        View* defaultFocus(View *oldFocus) override;

    protected:
        void customSpacing(View *current, View *next, int *spacing) override;
};
