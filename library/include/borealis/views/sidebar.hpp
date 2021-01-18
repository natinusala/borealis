/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2021  natinusala
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

#include <borealis/views/box.hpp>
#include <borealis/views/label.hpp>
#include <borealis/views/rectangle.hpp>
#include <borealis/views/scrolling_frame.hpp>

namespace brls
{

class SidebarItem;

class SidebarSeparator : public View
{
  public:
    SidebarSeparator();

    void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) override;
};

class SidebarItemGroup
{
  public:
    void add(SidebarItem* item);
    void setActive(SidebarItem* item);

  private:
    std::vector<SidebarItem*> items;
};

class SidebarItem : public Box
{
  public:
    SidebarItem();

    void onFocusGained() override;
    void onFocusLost() override;

    void setGroup(SidebarItemGroup* group);

    void setLabel(std::string label);

    void setActive(bool active);

    GenericEvent* getActiveEvent();

  private:
    Rectangle* accent;
    Label* label;

    GenericEvent activeEvent;

    SidebarItemGroup* group;

    bool active = false;
};

class Sidebar : public ScrollingFrame
{
  public:
    Sidebar();

    /**
     * Adds an item to this sidebar. The given callback will be called
     * when the item becomes active.
     */
    void addItem(std::string label, GenericEvent::Callback focusCallback);

    /**
     * Adds a separator to this sidebar.
     */
    void addSeparator();

    static View* create();

  private:
    SidebarItemGroup group;

    Box* contentBox;
};

} // namespace brls
