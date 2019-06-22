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

#include <TableView.hpp>

void TableView::addRow(TableViewRowType type, string label, string value)
{
    TableViewRow *row = new TableViewRow(type, label, value);
    this->rows.push_back(row);
}

void TableView::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    Theme *theme = ctx->theme;
    unsigned yAdvance = 0;

    for (size_t i = 0; i < this->rows.size(); i++)
    {
        bool even = i % 2 == 0;
        TableViewRow *row = this->rows[i];

        // Get appearance
        unsigned indent     = 0;
        unsigned height     = 0;
        unsigned fontSize   = 0;
        NVGcolor backgroundColor;
        NVGcolor textColor;

        switch (row->getType())
        {
            case TableViewRowType::HEADER:
                indent      = 0;
                height      = style->TableViewRow.headerHeight;
                textColor   = theme->textColor;
                fontSize    = style->TableViewRow.headerTextSize;
                break;
            case TableViewRowType::BODY:
                indent      = style->TableViewRow.bodyIndent;
                height      = style->TableViewRow.bodyHeight;
                textColor   = theme->tableBodyTextColor;
                fontSize    = style->TableViewRow.bodyTextSize;
                break;
        }

        backgroundColor = even ? theme->tableEvenBackgroundColor : transparent;

        // Background
        nvgFillColor(vg, a(backgroundColor));
        nvgBeginPath(vg);
        nvgRect(vg, x + indent, y + yAdvance, width - indent, height);
        nvgFill(vg);

        // Text
        nvgFillColor(vg, a(textColor));
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgFontSize(vg, fontSize);

        // Label
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgBeginPath(vg);
        nvgText(vg, x + indent + style->TableViewRow.padding, y + yAdvance + height/2, row->getLabel()->c_str(), nullptr);

        // Value
        nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        nvgBeginPath(vg);
        nvgText(vg, x + width - style->TableViewRow.padding, y + yAdvance + height/2, row->getValue()->c_str(), nullptr);

        yAdvance += height;
    }
}

void TableView::layout(NVGcontext* vg, Style *style, FontStash *stash)
{
    unsigned height = 0;

    for (TableViewRow *row : this->rows)
    {
        switch (row->getType())
        {
            case TableViewRowType::HEADER:
                height += style->TableViewRow.headerHeight;
                break;
            case TableViewRowType::BODY:
                height += style->TableViewRow.bodyHeight;
                break;
        }
    }

    this->setHeight(height);
}

TableView::~TableView()
{
    for (TableViewRow *row : this->rows)
    {
        delete row;
    }
}

TableViewRow::TableViewRow(TableViewRowType type, string label, string value) : type(type), label(label), value(value)
{

}

string* TableViewRow::getLabel()
{
    return &this->label;
}

string* TableViewRow::getValue()
{
    return &this->value;
}

TableViewRowType TableViewRow::getType()
{
    return this->type;
}