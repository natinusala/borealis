/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala
    Copyright (C) 2021  XITRIX

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

#include <borealis/core/touch/gesture_recognizer.hpp>

namespace brls
{

GestureState GestureRecognizer::recognitionLoop(TouchState touch, View* view)
{
    return GestureState::FAILED;
}

void GestureRecognizer::interrupt(bool onlyIfUnsureState)
{
    if (onlyIfUnsureState && this->state != GestureState::UNSURE) 
        return;

    this->state = GestureState::INTERRUPTED;
}

bool GestureRecognizer::soundOnTouch()
{
    return true;
}

}