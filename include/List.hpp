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

#include <BoxLayout.hpp>
#include <Label.hpp>

// A list item
// TODO: Use a Label with integrated ticker
class ListItem : public View
{
    private:
        string label;
        string value;

        bool drawTopSeparator = true;

        Label *sublabelView = nullptr;

        function<void(View*)> onClickListener;

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

        void setValue(string value);

        void setOnClickListener(function<void(View*)> listener);

        ~ListItem();
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