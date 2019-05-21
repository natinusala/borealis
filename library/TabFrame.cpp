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

#include <TabFrame.hpp>
#include <BoxLayout.hpp>
#include <Rectangle.hpp>
#include <Sidebar.hpp>

TabFrame::TabFrame()
{
    //Create sidebar
    this->sidebar = new Sidebar();

    // Setup content view
    this->layout = new BoxLayout(BOXLAYOUT_HORIZONTAL);
    layout->addView(sidebar);

    this->setContentView(layout);
}

void TabFrame::addTab(string label, View *view)
{
    this->sidebar->addItem(label);
    // TODO: Do something with the view
}

void TabFrame::addSeparator()
{
    this->sidebar->addSeparator();
}

View* TabFrame::requestFocus(FocusDirection direction)
{
    // Give focus to the sidebar by default
    // TODO: Give focus to the right pane instead
    if (direction == FOCUSDIRECTION_NONE)
        return this->sidebar->requestFocus(direction);

    // Let the layout do its thing
    return this->layout->requestFocus(direction);
}

TabFrame::~TabFrame()
{
    delete this->sidebar;
}