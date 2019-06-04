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

#include <List.hpp>
#include <Application.hpp>

// TODO: Scrollbar
// TODO: Spacing item (defaults to 60px) (to have make groups of items)

List::List() : BoxLayout(BoxLayoutOrientation::VERTICAL)
{
    Style *style = Application::getStyle();
    this->setMargins(style->List.marginTopBottom, style->List.marginLeftRight, style->List.marginTopBottom, style->List.marginLeftRight);
    this->setSpacing(style->List.spacing);
}

void List::customSpacing(View *current, View *next, int *spacing)
{
    // Don't add spacing to the first list item
    // if it doesn't have a sublabel and the second one is a
    // list item too
    if (ListItem *currentItem = dynamic_cast<ListItem*>(current))
    {
        if (ListItem *nextItem = dynamic_cast<ListItem*>(next))
        {
            if (!currentItem->hasSubLabel())
            {
                *spacing = 2;
                nextItem->setDrawTopSeparator(false);
            }
        }
    }
}


View* List::defaultFocus(View *oldFocus)
{
    if (this->focusedIndex >= 0 && this->focusedIndex < this->children.size())
    {
        View *newFocus = this->children[this->focusedIndex]->view->requestFocus(FocusDirection::NONE, oldFocus);
        if (newFocus)
            return newFocus;
    }

    return BoxLayout::defaultFocus(oldFocus);
}

ListItem::ListItem(string label, string sublabel) : label(label)
{
    Style *style = Application::getStyle();
    this->setHeight(style->List.Item.height);

    if (sublabel != "")
        this->sublabelView = new Label(LabelStyle::SUBLABEL, sublabel, true);
}

void ListItem::setParent(View *parent)
{
    View::setParent(parent);
    if (this->sublabelView)
        this->sublabelView->setParent(parent);
}

void ListItem::layout(NVGcontext *vg, Style *style)
{
    if (this->sublabelView)
    {
        this->height = style->List.Item.height;
        this->sublabelView->setBoundaries(this->x + style->List.Item.sublabelIndent, this->y + this->height + style->List.Item.sublabelSpacing, this->width - style->List.Item.sublabelIndent*2, 0);
        this->sublabelView->layout(vg, style); // we must call layout directly
        this->height += this->sublabelView->getHeight() + style->List.Item.sublabelSpacing;
    }
}

void ListItem::getHighlightInsets(unsigned *top, unsigned *right, unsigned *bottom, unsigned *left)
{
    Style *style = Application::getStyle();
    View::getHighlightInsets(top, right, bottom, left);
    if (sublabelView)
        *bottom = -(sublabelView->getHeight() + style->List.Item.sublabelSpacing);
}

void ListItem::setValue(string value)
{
    this->value = value;
}

void ListItem::setDrawTopSeparator(bool draw)
{
    this->drawTopSeparator = draw;
}

View* ListItem::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    return this;
}

// TODO: Adjust font size
void ListItem::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    unsigned baseHeight = style->List.Item.height;

    // Sublabel
    if (this->sublabelView)
        this->sublabelView->frame(ctx);

    // Value
    nvgFillColor(vg, a(ctx->theme->listItemValueColor));
    nvgFontSize(vg, style->List.Item.valueSize);
    nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + width - style->List.Item.padding, y + baseHeight/2, this->value.c_str(), nullptr);

    // Label
    nvgFillColor(vg, a(ctx->theme->textColor));
    nvgFontSize(vg, style->List.Item.textSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + style->List.Item.padding, y + baseHeight/2, this->label.c_str(), nullptr);

    // Separators
    // Offset by one to be hidden by highlight
    nvgFillColor(vg, a(ctx->theme->listItemSeparatorColor));

    // Top
    if (this->drawTopSeparator)
    {
        nvgBeginPath(vg);
        nvgRect(vg, x, y - 1, width, 1);
        nvgFill(vg);
    }

    // Bottom
    nvgBeginPath(vg);
    nvgRect(vg, x, y + 1 + baseHeight, width, 1);
    nvgFill(vg);
}

bool ListItem::hasSubLabel()
{
    return this->sublabelView;
}

ListItem::~ListItem()
{
    if (this->sublabelView)
        delete this->sublabelView;
}