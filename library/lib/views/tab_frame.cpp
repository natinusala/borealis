/*
    Copyright 2019-2021 natinusala
    Copyright 2019 WerWolv
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

#include <borealis/core/logger.hpp>
#include <borealis/core/util.hpp>
#include <borealis/views/tab_frame.hpp>

const std::string tabFrameContentXML = R"xml(
    <brls:Box
        width="auto"
        height="auto"
        axis="row"
        marginRight="@style/brls/applet_frame/padding_sides">

        <brls:Sidebar
            id="brls/tab_frame/sidebar"
            width="@style/brls/tab_frame/sidebar_width"
            height="auto" />

        <!-- Content will be injected here with grow="1.0" -->

    </brls:Box>
)xml";

namespace brls
{

TabFrame::TabFrame()
{
    View* contentView = View::createFromXMLString(tabFrameContentXML);
    this->setContentView(contentView);
}

void TabFrame::addTab(std::string label, TabViewCreator creator)
{
    this->sidebar->addItem(label, [this, creator](brls::View* view) {
        // Only trigger when the sidebar item gains focus
        if (!view->isFocused())
            return;

        Box* contentView = (Box*)this->contentView;

        // Remove the existing tab if it exists
        if (this->activeTab)
        {
            contentView->removeView(this->activeTab); // will call willDisappear and delete
            this->activeTab = nullptr;
        }

        // Add the new tab
        View* newContent = creator();

        if (!newContent)
            return;

        newContent->setGrow(1.0f);
        contentView->addView(newContent); // addView calls willAppear

        this->activeTab = newContent;
    });
}

void TabFrame::addSeparator()
{
    this->sidebar->addSeparator();
}

void TabFrame::handleXMLElement(tinyxml2::XMLElement* element)
{
    std::string name = element->Name();

    if (name == "brls:Tab")
    {
        const tinyxml2::XMLAttribute* labelAttribute = element->FindAttribute("label");

        if (!labelAttribute)
            fatal("\"label\" attribute missing from \"" + name + "\" tab");

        std::string label = View::getStringXMLAttributeValue(labelAttribute->Value());

        tinyxml2::XMLElement* viewElement = element->FirstChildElement();

        if (viewElement)
        {
            this->addTab(label, [viewElement] {
                return View::createFromXMLElement(viewElement);
            });

            if (viewElement->NextSiblingElement())
                fatal("\"brls:Tab\" can only contain one child element");
        }
        else
        {
            this->addTab(label, [] { return nullptr; });
        }
    }
    else if (name == "brls:Separator")
    {
        this->addSeparator();
    }
    else
    {
        fatal("Unknown child element \"" + name + "\" for \"brls:Tab\"");
    }
}

View* TabFrame::create()
{
    return new TabFrame();
}

} // namespace brls
