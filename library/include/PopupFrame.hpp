/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
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

#include <AppletFrame.hpp>
#include <View.hpp>
#include <List.hpp>

#include <string>

using namespace std;

class PopupFrame : public View
{
    private:
        PopupFrame(string title, unsigned char *imageBuffer, size_t imageBufferSize, AppletFrame *contentView, string subTitleLeft = "", string subTitleRight = "");
        PopupFrame(string title, string imagePath, AppletFrame *contentView, string subTitleLeft = "", string subTitleRight = "");
        PopupFrame(string title, AppletFrame *contentView, string subTitleLeft = "", string subTitleRight = "");

        AppletFrame *contentView = nullptr;

    protected:
        unsigned getShowAnimationDuration() override;

    public:
        ~PopupFrame();

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;
        void layout(NVGcontext* vg, Style *style, FontStash *stash) override;
        View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp = false) override;
        bool onCancel() override;
        void willAppear() override;
        void willDisappear() override;

        static void open(string title, unsigned char *imageBuffer, size_t imageBufferSize, AppletFrame *contentView, string subTitleLeft = "", string subTitleRight = "");
        static void open(string title, string imagePath, AppletFrame *contentView, string subTitleLeft = "", string subTitleRight = "");
        static void open(string title, AppletFrame *contentView, string subTitleLeft = "", string subTitleRight = "");

        bool isTranslucent() override
        {
            return true;
        }
};