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

// TODO: Scrollbar
// TODO: Spacing item (defaults to 60px) (to have make groups of items)

List::List() : BoxLayout(BOXLAYOUT_VERTICAL)
{
    Style *style = getStyle();
    this->setMargins(style->List.marginTopBottom, style->List.marginRight, style->List.marginTopBottom, style->List.marginLeft);
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

void ListItem::setValue(string value)
{
    this->value = value;
}

void ListItem::setDrawTopSeparator(bool draw)
{
    this->drawTopSeparator = draw;
}

ListItem::ListItem(string label, string sublabel) : label(label), sublabel(sublabel)
{
    Style *style = getStyle();
    this->setHeight(style->List.Item.height);

    // TODO: Change height if there is a sublabel
}

View* ListItem::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    return this;
}

// TODO: Adjust font size
void ListItem::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    // TODO: Word wrapped sublabel

    // Value
    nvgFillColor(vg, ctx->theme->listItemValueColor);
    nvgFontSize(vg, style->List.Item.textSize);
    nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + width - style->List.Item.padding, y + height/2, this->value.c_str(), nullptr);

    // Label
    nvgFillColor(vg, ctx->theme->textColor);
    nvgFontSize(vg, style->List.Item.textSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + style->List.Item.padding, y + height/2, this->label.c_str(), nullptr);

    // Separators
    // Offset by one to be hidden by highlight
    nvgFillColor(vg, ctx->theme->listItemSeparatorColor);

    // Top
    if (this->drawTopSeparator)
    {
        nvgBeginPath(vg);
        nvgRect(vg, x, y - 1, width, 1);
        nvgFill(vg);
    }

    // Bottom
    nvgBeginPath(vg);
    nvgRect(vg, x, y + 1 + height, width, 1);
    nvgFill(vg);
}

bool ListItem::hasSubLabel()
{
    return this->sublabel != "";
}