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
#define SIDEBAR_SPACING 18

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

void Sidebar::draw(FrameContext *ctx)
{
    // Draw items
    BoxLayout::draw(ctx);
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

#define SIDEBARITEM_HEIGHT      52
#define SIDEBARITEM_TEXT_SIZE   22

#define SIDEBARITEM_TEXT_OFFSET_X 14

SidebarItem::SidebarItem(string label, Sidebar *sidebar) : label(label), sidebar(sidebar)
{
    this->setHeight(SIDEBARITEM_HEIGHT);
}

void SidebarItem::draw(FrameContext *ctx)
{
    // Label
    nvgFillColor(ctx->vg, this->active ? ctx->theme->activeTabColor : ctx->theme->textColor);
    nvgFontSize(ctx->vg, SIDEBARITEM_TEXT_SIZE);
    nvgTextAlign(ctx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(ctx->vg, this->x + SIDEBARITEM_TEXT_OFFSET_X, this->y + this->height / 2, this->label.c_str(), nullptr);

    // Active marker
    if (this->active)
    {
        nvgFillColor(ctx->vg, ctx->theme->activeTabColor);
        nvgBeginPath(ctx->vg);
        nvgRect(ctx->vg, this->x, this->y, 4, SIDEBARITEM_HEIGHT);
        nvgFill(ctx->vg);
    }
}

void SidebarItem::setActive(bool active)
{
    this->active = active;
}

void SidebarSeparator::draw(FrameContext *ctx)
{
    nvgFillColor(ctx->vg, ctx->theme->sidebarSeparatorColor);
    nvgBeginPath(ctx->vg);
    nvgRect(ctx->vg, this->x, this->y + this->height / 2, this->width, 1);
    nvgFill(ctx->vg);
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