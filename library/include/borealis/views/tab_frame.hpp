/*
    Copyright 2019-2021 natinusala
    Copyright 2019 p-sam

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

#include <borealis/core/bind.hpp>
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
    BRLS_BIND(Sidebar, sidebar, "brls/tab_frame/sidebar");

    View* activeTab = nullptr;
};

} // namespace brls
