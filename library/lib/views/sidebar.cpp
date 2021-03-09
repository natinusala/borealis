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

#include <borealis/core/application.hpp>
#include <borealis/core/i18n.hpp>
#include <borealis/views/sidebar.hpp>

using namespace brls::literals;

namespace brls
{

const std::string sidebarItemXML = R"xml(
    <brls:Box
        width="auto"
        height="@style/brls/sidebar/item_height"
        focusable="true" >

        <brls:Rectangle
            id="brls/sidebar/item_accent"
            width="@style/brls/sidebar/item_accent_rect_width"
            height="auto"
            visibility="invisible"
            color="@theme/brls/sidebar/active_item"
            marginTop="@style/brls/sidebar/item_accent_margin_top_bottom"
            marginBottom="@style/brls/sidebar/item_accent_margin_top_bottom"
            marginLeft="@style/brls/sidebar/item_accent_margin_sides"
            marginRight="@style/brls/sidebar/item_accent_margin_sides" />

        <brls:Label
            id="brls/sidebar/item_label"
            width="auto"
            height="auto"
            grow="1.0"
            fontSize="@style/brls/sidebar/item_font_size"
            marginTop="@style/brls/sidebar/item_accent_margin_top_bottom"
            marginBottom="@style/brls/sidebar/item_accent_margin_top_bottom"
            marginRight="@style/brls/sidebar/item_accent_margin_sides" />

    </brls:Box>
)xml";

SidebarItem::SidebarItem()
    : Box(Axis::ROW)
{
    this->inflateFromXMLString(sidebarItemXML);

    this->registerStringXMLAttribute("label", [this](std::string value) {
        this->setLabel(value);
    });

    this->setFocusSound(SOUND_FOCUS_SIDEBAR);

    this->registerAction(
        "brls/hints/ok"_i18n, BUTTON_A, [this](View* view) {
            Application::onControllerButtonPressed(BUTTON_RIGHT, false);
            return true;
        },
        false, SOUND_CLICK_SIDEBAR);
}

void SidebarItem::setActive(bool active)
{
    if (active == this->active)
        return;

    Theme theme = Application::getTheme();

    if (active)
    {
        this->activeEvent.fire(this);

        this->accent->setVisibility(Visibility::VISIBLE);
        this->label->setTextColor(theme["brls/sidebar/active_item"]);
    }
    else
    {
        this->accent->setVisibility(Visibility::INVISIBLE);
        this->label->setTextColor(theme["brls/text"]);
    }

    this->active = active;
}

void SidebarItem::onFocusGained()
{
    Box::onFocusGained();

    if (this->group)
        this->group->setActive(this);
}

void SidebarItem::onFocusLost()
{
    Box::onFocusLost();
}

void SidebarItem::setGroup(SidebarItemGroup* group)
{
    this->group = group;

    if (group)
        group->add(this);
}

GenericEvent* SidebarItem::getActiveEvent()
{
    return &this->activeEvent;
}

void SidebarItem::setLabel(std::string label)
{
    this->label->setText(label);
}

Sidebar::Sidebar()
{
    Style style = Application::getStyle();

    this->setScrollingBehavior(ScrollingBehavior::CENTERED);
    this->setBackground(ViewBackground::SIDEBAR);

    // Create content box
    this->contentBox = new Box(Axis::COLUMN);

    this->contentBox->setPadding(
        style["brls/sidebar/padding_top"],
        style["brls/sidebar/padding_right"],
        style["brls/sidebar/padding_bottom"],
        style["brls/sidebar/padding_left"]);

    this->setContentView(this->contentBox);
}

void Sidebar::addItem(std::string label, GenericEvent::Callback focusCallback)
{
    SidebarItem* item = new SidebarItem();
    item->setGroup(&this->group);
    item->setLabel(label);
    item->getActiveEvent()->subscribe(focusCallback);

    this->contentBox->addView(item);
}

void Sidebar::addSeparator()
{
    this->contentBox->addView(new SidebarSeparator());
}

View* Sidebar::create()
{
    return new Sidebar();
}

void SidebarItemGroup::add(SidebarItem* item)
{
    this->items.push_back(item);
}

void SidebarItemGroup::setActive(SidebarItem* active)
{
    for (SidebarItem* item : this->items)
    {
        if (item == active)
            item->setActive(true);
        else
            item->setActive(false);
    }
}

SidebarSeparator::SidebarSeparator()
{
    Style style = Application::getStyle();

    this->setHeight(style["brls/sidebar/separator_height"]);
}

void SidebarSeparator::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    float midY = y + height / 2;

    nvgBeginPath(vg);
    nvgFillColor(vg, ctx->theme["brls/sidebar/separator"]);
    nvgRect(vg, x, midY, width, 1);
    nvgFill(vg);
}

} // namespace brls
