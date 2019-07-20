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

#include <string>
#include <functional>

using namespace std;

bool askForKeyboardInputString(function<void(string)> f, string headerText = "", string subText = "", int maxStringLength = 32, string initialText = "");
bool askForKeyboardInputInteger(function<void(int)> f, string headerText = "", string subText = "", int maxStringLength = 32, string initialText = "", string leftButton = "", string rightButton = "");