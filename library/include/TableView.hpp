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

#pragma once

#include <View.hpp>

#include <string>

// The type of a TableView Row
enum class TableViewRowType
{
    HEADER = 0,
    BODY
};

// A TableView row
class TableViewRow
{
    private:
        TableViewRowType type;

        string label;
        string value;

    public:
        TableViewRow(TableViewRowType type, string label, string value = "");

        string* getLabel();
        string *getValue();
        TableViewRowType getType();
};

// A simple, static two-columns table, as seen in
// the Settings app (Internet connection details)
// All rows must be added before adding the view
// to a layout (it's static)
class TableView : public View
{
    private:
        vector<TableViewRow*> rows;

    public:
        ~TableView();

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;
        void layout(NVGcontext* vg, Style *style, FontStash *stash) override;

        void addRow(TableViewRowType type, string label, string value = "");
};