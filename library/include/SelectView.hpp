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

#include <View.hpp>
#include <List.hpp>

#include <string>

using namespace std;

// Called when the user has selected a value
//
// Parameter is either the selected value index
// or -1 if the user cancelled
//
// Assume that the SelectView is deleted
// as soon as this function is called
typedef function<void(int)> SelectListener;

// Allows the user to select between multiple
// values (like a dropdown menu)
// Use SelectView::open()
class SelectView : public View
{
    private:
        SelectView(string title, vector<string> values, SelectListener listener, unsigned selected = 0);

        string title;

        int valuesCount;
        unsigned selectedValue;

        SelectListener listener;

        List *list;

        float topOffset; // for slide in animation

    protected:
        unsigned getShowAnimationDuration() override;

    public:
        ~SelectView();

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;
        void layout(NVGcontext* vg, Style *style, FontStash *stash) override;
        View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp = false) override;
        bool onCancel() override;
        void show(function<void(void)> cb) override;

        static void open(string title, vector<string> values, SelectListener listener, int selected = -1);

        bool isTranslucent() override
        {
            return true || View::isTranslucent();
        }
};
