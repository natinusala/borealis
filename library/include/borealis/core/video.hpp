/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala

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

#include <nanovg.h>

// A VideoContext is responsible for providing a nanovg context for the app
// (so by extension it manages all the graphics state as well as the window / context).
// The VideoContext implementation must also provide the nanovg implementation. As such, there
// can only be one VideoContext linked at any time in the binary.
// Context creation and teardown can be done in the constructor and destructor.
class VideoContext
{
  public:
    virtual ~VideoContext() {};

    /**
     * Called at the beginning of every frame to clear the window
     */
    virtual void clear(NVGcolor color) = 0;

    /**
     * Called at the beginning of every frame to begin it.
     */
    virtual void beginFrame() = 0;

    /**
     * Called at the end of every frame to end it (swap buffers...).
     */
    virtual void endFrame() = 0;

    /**
     * Can be called by the application to reset the graphics
     * state, in case there is a need to use the graphics API
     * directly (for instance direct OpenGL calls).
     */
    virtual void resetState() = 0;

    virtual NVGcontext* getNVGContext() = 0;
};
