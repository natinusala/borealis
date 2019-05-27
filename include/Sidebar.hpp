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

#include <vector>
#include <string>

using namespace std;

// A sidebar with multiple tabs

class SidebarSeparator : public View
{
    public:
        SidebarSeparator();

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;

        View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp = false) override
        {
            return nullptr;
        }
};

class Sidebar;

// TODO: Use a Label view with integrated ticker
class SidebarItem : public View
{
    private:
        string label;
        bool active = false;

        Sidebar *sidebar        = nullptr;
        View *associatedView    = nullptr;

    public:
        SidebarItem(string label, Sidebar *sidebar);

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;
        View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp = false) override;

        void setActive(bool active);
        bool isActive();

        void onFocusGained() override;

        void setAssociatedView(View *view);
        View* getAssociatedView();

        ~SidebarItem();
};

// TODO: Add a style with icons, make it collapsible?
class Sidebar : public BoxLayout
{
    private:
        SidebarItem *currentActive = nullptr;

    protected:
        virtual View* defaultFocus(View *oldFocus) override;

    public:
        Sidebar();

        SidebarItem* addItem(string label, View *view);
        void addSeparator();

        void setActive(SidebarItem *item);
};