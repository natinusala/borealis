/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  natinusala

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

// A special kind of view that has additional callbacks around
// the application frame() method (before and after the nanovg frame)
// An application can only have one background
class Background : public View
{
  public:
    /**
          * Called before the nanovg frame starts
          */
    virtual void preFrame() = 0;

    /**
         * Called after the end of the nanovg frame
         */
    virtual void postFrame() = 0;
};

} // namespace brls
