/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  WerWolv

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

#include <string>
#include <functional>

namespace brls
{

class View;

typedef std::function<bool(void)> ActionListener;

// These values must match GLFW's button codes
// This is also the reason why ZL and ZR do not exist here. GLFW doesn't know them
enum class Key
{
    A         = 0,
    B         = 1,
    X         = 2,
    Y         = 3,
    LSTICK    = 9,
    RSTICK    = 10,
    L         = 4,
    R         = 5,
    PLUS      = 7,
    MINUS     = 6,
    DLEFT     = 14,
    DUP       = 11,
    DRIGHT    = 12,
    DDOWN     = 13,
};

struct Action {
  Key key;
  
  std::string hintText;
  bool available;
  bool hidden;
  ActionListener actionListener;

  bool operator==(const Key other)
  {
    return this->key == other;
  }
};

} // namespace brls