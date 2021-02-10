/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2021  natinusala
    Copyright (C) 2019  WerWolv
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

    this->sidebar = (Sidebar*)this->getView("brls/tab_frame/sidebar");
}

// TODO: change getDefaultFocus to try to focus the right pane instead
// TODO: remember focus when doing right side -> left sidebar -> right side (add a rememberFocus attribute to Box)
// TODO: make sidebar items clickable, go to the right side when clicked

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
