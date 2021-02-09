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

#include <borealis/core/application.hpp>
#include <borealis/core/i18n.hpp>
#include <borealis/views/sidebar.hpp>

using namespace brls::literals;

namespace brls
{

// TODO: restore OK / Back behavior, as well as focus conservation

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

    this->accent = (Rectangle*)this->getView("brls/sidebar/item_accent");
    this->label  = (Label*)this->getView("brls/sidebar/item_label");

    this->registerStringXMLAttribute("label", [this](std::string value) {
        this->setLabel(value);
    });

    this->setFocusSound(SOUND_FOCUS_SIDEBAR);

    this->registerAction(
        "brls/hints/ok"_i18n, BUTTON_A, [this](View* view) {
            // TODO: find a way to not play the focus sound when calling that
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
