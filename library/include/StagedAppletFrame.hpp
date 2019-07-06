/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  Billy Laws

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
#include <Sidebar.hpp>

#include <vector>
#include <string>

using namespace std;

// An applet frame for implementing a stage based app with a progress display in top right

class StagedAppletFrame : public AppletFrame
{
    public:
        StagedAppletFrame();

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;

        void addStage(View *view);
        void nextStage();
        void previousStage();
        void enterStage(int index);

        ~StagedAppletFrame();

    private:
        size_t currentStage = 0;
        vector<View*> stageViews;
};
