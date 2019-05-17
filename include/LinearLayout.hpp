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

#include <vector>

using namespace std;

// A linear (horizontal or vertical) layout

typedef enum
{
    VERTICAL = 0,
    HORIZONTAL
} LinearLayoutOrientation;

class LinearLayout : public View
{
    private:
        LinearLayoutOrientation orientation;

        vector<View*> children;

    public:
        LinearLayout(LinearLayoutOrientation orientation);

        void frame(FrameContext *ctx) override;
        void layout() override;
};