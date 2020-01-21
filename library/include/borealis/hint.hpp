/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  WerWolv

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

#include <borealis/view.hpp>

namespace brls
{

// A button input hint
class Hint : public View
{
  private:
    bool animateHint;

  public:
    Hint();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;

    View* requestFocus(FocusDirection direction, View* oldFocus, bool fromUp = false) override
    {
        return nullptr;
    }

    void setAnimateHint(bool animate)
    {
        this->animateHint = animate;
    }

    static void handleInput(char button);
};

} // namespace brls
