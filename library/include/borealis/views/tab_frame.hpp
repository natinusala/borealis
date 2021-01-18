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

#include <borealis/views/applet_frame.hpp>
#include <borealis/views/sidebar.hpp>
#include <functional>

namespace brls
{

typedef std::function<View*(void)> TabViewCreator;

// An applet frame containing a sidebar on the left with multiple tabs which content is showing on the right.
// Only one tab is kept in memory at all times : when switching, the current tab is freed before the the new one is instantiated.
class TabFrame : public AppletFrame
{
  public:
    TabFrame();

    void handleXMLElement(tinyxml2::XMLElement* element) override;

    void addTab(std::string label, TabViewCreator creator);
    void addSeparator();

    static View* create();

  private:
    Sidebar* sidebar;

    View* activeTab = nullptr;
};

} // namespace brls
