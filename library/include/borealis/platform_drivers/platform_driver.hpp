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

#include <utility>
#include <string>

struct NVGcontext;

namespace brls::drv
{

    enum class Key : unsigned short
    {
        A      = (1 << 0),
        B      = (1 << 1),
        X      = (1 << 2),
        Y      = (1 << 3),
        LSTICK = (1 << 4),
        RSTICK = (1 << 5),
        L      = (1 << 6),
        R      = (1 << 7),
        PLUS   = (1 << 8),
        MINUS  = (1 << 9),
        DLEFT  = (1 << 10),
        DUP    = (1 << 11),
        DRIGHT = (1 << 12),
        DDOWN  = (1 << 13),
    };

    extern unsigned short operator|(const Key lhs, const Key rhs);
    extern unsigned short operator&(const Key lhs, const Key rhs);

    class PlatformDriver {
    public:
        virtual ~PlatformDriver() = default;

        virtual bool initialize(const std::string &title, unsigned int windowWidth, unsigned int windowHeight) = 0;
        virtual bool exit() = 0; 
         
        virtual bool update() = 0;
        virtual void frame() = 0;
        virtual void swapBuffers() = 0;

        virtual bool isAnyKeyDown() const = 0;
        virtual bool haveKeyStatesChanged() const = 0;
        virtual bool isKeyDown(const Key &key) const = 0;
        virtual bool isKeyUp(const Key &key) const = 0;
        virtual bool isKeyHeld(const Key &key) const = 0;
 
        virtual std::pair<int, int> getTouchPosition() const = 0;
        virtual int getTouchCount() const = 0;
 
        NVGcontext* getNVGContext();

        unsigned int getWindowWidth();
        unsigned int getWindowHeight();
    protected:
        NVGcontext* vg;

        unsigned int windowWidth, windowHeight;
    };

} // namespace brls::drv
