/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
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

#include <borealis/platform_drivers/platform_driver.hpp>

namespace brls::drv
{
    unsigned short operator|(const Key lhs, const Key rhs)
    {
        return static_cast<unsigned short>(lhs) | static_cast<unsigned short>(rhs);
    }

    unsigned short operator&(const Key lhs, const Key rhs)
    {
        return static_cast<unsigned short>(lhs) & static_cast<unsigned short>(rhs);
    }


    NVGcontext* PlatformDriver::getNVGContext()
    { 
        return this->vg;
    }

    unsigned int PlatformDriver::getWindowWidth()
    { 
        return this->windowWidth;
    }

    unsigned int PlatformDriver::getWindowHeight()
    {
        return this->windowHeight;
    }

} // namespace brls::drv