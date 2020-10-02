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

#include <functional>
#include <string>
#include <nvgwrapper.hpp>
#include <borealis/logger.hpp>

namespace brls
{

class View;

typedef std::function<bool(void)> ActionListener;

// ZL and ZR do not exist here because GLFW doesn't know them
enum class Key
{
    A      = KEY_A,
    B      = KEY_B,
    X      = KEY_X,
    Y      = KEY_Y,
    LSTICK = KEY_LSTICK,
    RSTICK = KEY_RSTICK,
    L      = KEY_L,
    R      = KEY_R,
    PLUS   = KEY_PLUS,
    MINUS  = KEY_MINUS,
    DLEFT  = KEY_DLEFT,
    DUP    = KEY_DUP,
    DRIGHT = KEY_DRIGHT,
    DDOWN  = KEY_DDOWN,
};

struct Action
{
    Key key;

    std::string hintText;
    bool available;
    bool hidden;
    ActionListener actionListener;

    bool operator==(const Key other)
    {
        Logger::info("keys: {} vs {}", this->key, other);
        return this->key == other;
    }
};

} // namespace brls
