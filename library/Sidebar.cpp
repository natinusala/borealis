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

#define SIDEBAR_WIDTH   410
#define SIDEBAR_SPACING 0

#define SIDEBAR_MARGIN_LEFT     88
#define SIDEBAR_MARGIN_RIGHT    30
#define SIDEBAR_MARGIN_TOP      40
#define SIDEBAR_MARGIN_BOTTOM   40

Sidebar::Sidebar() : BoxLayout(BOXLAYOUT_VERTICAL)
{
    this->setWidth(SIDEBAR_WIDTH);
    this->setSpacing(SIDEBAR_SPACING);
    this->setMargins(SIDEBAR_MARGIN_TOP, SIDEBAR_MARGIN_RIGHT, SIDEBAR_MARGIN_BOTTOM, SIDEBAR_MARGIN_LEFT);
    this->setBackground(BACKGROUND_SIDEBAR);
}

View* Sidebar::defaultFocus()
{
    for (unsigned i = 0; i < this->children.size(); i++)
    {
        View *view = this->children[i]->view;
        if (SidebarItem *item = dynamic_cast<SidebarItem*>(view))
        {
            if (item->isActive())
            {
                View *newFocus = this->children[i]->view->requestFocus(FOCUSDIRECTION_NONE);
                if (newFocus)
                {
                    this->focusedIndex = i;
                    return newFocus;
                }
                break;
            }
        }
    }

    return BoxLayout::defaultFocus();
}

void Sidebar::addItem(string label)
{
    SidebarItem *item = new SidebarItem(label, this);
    if (this->isEmpty())
    {
        setActive(item);
    }

    this->addView(item);
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

#define SIDEBARITEM_HEIGHT      70
#define SIDEBARITEM_TEXT_SIZE   22
#define SIDEBARITEM_PADDING         9

#define SIDEBARITEM_TEXT_OFFSET_X   14
#define SIDEBARITEM_ACTIVE_WIDTH    4

#define SIDEBARITEM_HIGHLIGHT_INSET 0

SidebarItem::SidebarItem(string label, Sidebar *sidebar) : label(label), sidebar(sidebar)
{
    this->setHeight(SIDEBARITEM_HEIGHT);
}

void SidebarItem::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    // Label
    nvgFillColor(vg, this->active ? ctx->theme->activeTabColor : ctx->theme->textColor);
    nvgFontSize(vg, SIDEBARITEM_TEXT_SIZE);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + SIDEBARITEM_TEXT_OFFSET_X + SIDEBARITEM_PADDING, y + height / 2, this->label.c_str(), nullptr);

    // Active marker
    if (this->active)
    {
        nvgFillColor(vg, ctx->theme->activeTabColor);
        nvgBeginPath(vg);
        nvgRect(vg, x + SIDEBARITEM_PADDING, y + SIDEBARITEM_PADDING, SIDEBARITEM_ACTIVE_WIDTH, SIDEBARITEM_HEIGHT - SIDEBARITEM_PADDING*2);
        nvgFill(vg);
    }
}

void SidebarItem::setActive(bool active)
{
    this->active = active;
}

#define SIDEBARSEPARATOR_HEIGHT 28

SidebarSeparator::SidebarSeparator()
{
    this->setHeight(SIDEBARSEPARATOR_HEIGHT);
}

void SidebarSeparator::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    nvgFillColor(vg, ctx->theme->sidebarSeparatorColor);
    nvgBeginPath(vg);
    nvgRect(vg, x, y + height / 2, width, 1);
    nvgFill(vg);
}

View* SidebarItem::requestFocus(FocusDirection direction, bool fromUp)
{
    return this;
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

unsigned SidebarItem::getHighlightInset()
{
    return SIDEBARITEM_HIGHLIGHT_INSET;
}