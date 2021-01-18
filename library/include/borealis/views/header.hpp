/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2021  natinusala
    Copyright (C) 2019  p-sam

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

#include <borealis/views/box.hpp>
#include <borealis/views/label.hpp>

namespace brls
{

// A simple header with a title, an optional subtitle, a rectangle on the left
// and a separator
class Header : public Box
{
  public:
    Header();

    void setTitle(std::string text);
    void setSubtitle(std::string text);

    static View* create();

  private:
    Label* title;
    Label* subtitle;
};

} // namespace brls
