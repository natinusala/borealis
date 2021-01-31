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

#pragma once

namespace brls
{

enum TouchClickState {
    START,
    STAY,
    END,
    NONE,
};

typedef struct TouchState
{
    TouchClickState state = NONE;
    double x, y;
} TouchState;

class TouchManager
{
  public:
    virtual ~TouchManager() { }

    /**
     * Called once every frame to fill the given TouchState struct with the touch state.
     */
    virtual void updateTouchState(TouchState* state) = 0;
};

}; // namespace brls