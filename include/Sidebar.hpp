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
        void draw(FrameContext *ctx) override;
        void layout() override { };

        View *requestFocus(FocusDirection direction, bool fromUp = false) override
        {
            return nullptr;
        }
};

class Sidebar;

class SidebarItem : public View
{
    private:
        string label;
        bool active = false;

        Sidebar *sidebar;

    public:
        SidebarItem(string label, Sidebar *sidebar);

        void draw(FrameContext *ctx) override;
        void layout() override { };
        View *requestFocus(FocusDirection direction, bool fromUp = false) override;

        void setActive(bool active);
        bool isActive();

        void onFocusGained() override;

        ~SidebarItem() { }
};

// TODO: Add a style with icons, make it collapsible?
// TODO: Turn this into a List view with a sidebar style
class Sidebar : public BoxLayout
{
    private:
        SidebarItem *currentActive = nullptr;

    protected:
        virtual View* defaultFocus() override;

    public:
        Sidebar();

        void draw(FrameContext *ctx) override;

        void addItem(string label);
        void addSeparator();

        void setActive(SidebarItem *item);
};