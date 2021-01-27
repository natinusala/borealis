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

#include <borealis/core/audio.hpp>
#include <borealis/core/input.hpp>
#include <functional>
#include <string>

namespace brls
{

class View;

typedef std::function<bool(View*)> ActionListener;

struct Action
{
    enum ControllerButton button;

    std::string hintText;
    bool available;
    bool hidden;
    enum Sound sound;
    ActionListener actionListener;

    bool operator==(const enum ControllerButton other)
    {
        return this->button == other;
    }
};

} // namespace brls
