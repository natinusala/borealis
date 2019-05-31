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

#include <Sidebar.hpp>

Sidebar::Sidebar() : BoxLayout(BoxLayoutOrientation::VERTICAL)
{
    Style *style = getStyle();

    this->setWidth(style->Sidebar.width);
    this->setSpacing(style->Sidebar.spacing);
    this->setMargins(style->Sidebar.marginTop, style->Sidebar.marginRight, style->Sidebar.marginBottom, style->Sidebar.marginLeft);
    this->setBackground(Background::SIDEBAR);
}

View* Sidebar::defaultFocus(View *oldFocus)
{
    for (unsigned i = 0; i < this->children.size(); i++)
    {
        View *view = this->children[i]->view;
        if (SidebarItem *item = dynamic_cast<SidebarItem*>(view))
        {
            if (item->isActive())
            {
                View *newFocus = this->children[i]->view->requestFocus(FocusDirection::NONE, oldFocus);
                if (newFocus)
                {
                    this->focusedIndex = i;
                    return newFocus;
                }
                break;
            }
        }
    }

    return BoxLayout::defaultFocus(oldFocus);
}

SidebarItem* Sidebar::addItem(string label, View *view)
{
    SidebarItem *item = new SidebarItem(label, this);
    item->setAssociatedView(view);
    if (this->isEmpty())
    {
        setActive(item);
    }

    this->addView(item);

    return item;
}

void Sidebar::addSeparator()
{
    SidebarSeparator *separator = new SidebarSeparator();
    this->addView(separator);
}

void Sidebar::setActive(SidebarItem *active)
{
    if (currentActive)
        currentActive->setActive(false);
    currentActive = active;
    active->setActive(true);
}

SidebarItem::SidebarItem(string label, Sidebar *sidebar) : label(label), sidebar(sidebar)
{
    Style *style = getStyle();
    this->setHeight(style->Sidebar.Item.height);
}

void SidebarItem::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    // Label
    nvgFillColor(vg, this->active ? ctx->theme->activeTabColor : ctx->theme->textColor);
    nvgFontSize(vg, style->Sidebar.Item.textSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + style->Sidebar.Item.textOffsetX + style->Sidebar.Item.padding, y + height / 2, this->label.c_str(), nullptr);

    // Active marker
    if (this->active)
    {
        nvgFillColor(vg, ctx->theme->activeTabColor);
        nvgBeginPath(vg);
        nvgRect(vg, x + style->Sidebar.Item.padding, y + style->Sidebar.Item.padding, style->Sidebar.Item.activeMarkerWidth, style->Sidebar.Item.height - style->Sidebar.Item.padding*2);
        nvgFill(vg);
    }
}

void SidebarItem::setActive(bool active)
{
    this->active = active;
}

SidebarSeparator::SidebarSeparator()
{
    Style *style = getStyle();
    this->setHeight(style->Sidebar.Separator.height);
}

void SidebarSeparator::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    nvgFillColor(vg, ctx->theme->sidebarSeparatorColor);
    nvgBeginPath(vg);
    nvgRect(vg, x, y + height / 2, width, 1);
    nvgFill(vg);
}

View* SidebarItem::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    return this;
}

void SidebarItem::setAssociatedView(View *view)
{
    this->associatedView = view;
}

bool SidebarItem::isActive()
{
    return this->active;
}

void SidebarItem::onFocusGained()
{
    View::onFocusGained();
    this->sidebar->setActive(this);
}

View* SidebarItem::getAssociatedView()
{
    return this->associatedView;
}

SidebarItem::~SidebarItem()
{
    if (this->associatedView)
        delete this->associatedView;
}