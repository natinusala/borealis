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

class SidebarItem : public View
{
    private:
        string label;

    public:
        SidebarItem(string label);

        void frame(FrameContext *ctx) override;
        void layout() override;

        ~SidebarItem() { }
};

// TODO: Add a style with icons, make it collapsible?
// TODO: Add separator items
class Sidebar : public BoxLayout
{
    private:
        vector<SidebarItem*> items;

    public:
        Sidebar();

        void frame(FrameContext *ctx) override;
};