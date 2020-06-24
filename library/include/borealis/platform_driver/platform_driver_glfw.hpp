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

#include <borealis/platform_driver/platform_driver.hpp>

struct GLFWwindow;

namespace brls::drv
{

    class PlatformDriverGLFW : public PlatformDriver {
    public:
        virtual bool initialize(const std::string &title, unsigned int windowWidth, unsigned int windowHeight);
        virtual bool exit(); 
         
        virtual bool update();
        virtual void frame();
        virtual void swapBuffers();

        virtual bool isAnyKeyDown() const;
        virtual bool haveKeyStatesChanged() const;
        virtual bool isKeyDown(const Key &key) const;
        virtual bool isKeyUp(const Key &key) const;
        virtual bool isKeyHeld(const Key &key) const;
 
        virtual std::pair<int, int> getTouchPosition() const;
        virtual int getTouchCount() const;
 
        virtual void playSoundById(const std::string &title) const;

    private:
        GLFWwindow *window = nullptr;

        unsigned int gamepadDown = 0, gamepadDownOld = 0;
    };

} // namespace brls::drv