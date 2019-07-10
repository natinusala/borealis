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

#include <View.hpp>
#include <Label.hpp>
#include <ProgressSpinner.hpp>

#define PROGRESS_DISPLAY_STYLE_SPINNER 1 << 0
#define PROGRESS_DISPLAY_STYLE_PERCENTAGE 1 << 0

// A progress bar with an optional spinner and percentage text.
class ProgressDisplay : public View
{
    public:
        ProgressDisplay(unsigned progressStyle = PROGRESS_DISPLAY_STYLE_PERCENTAGE | PROGRESS_DISPLAY_STYLE_SPINNER);

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;
        void layout(NVGcontext* vg, Style *style, FontStash *stash) override;
        void willAppear() override;
        void willDisappear() override;

        View* requestFocus(FocusDirection direction, View *oldFocus, bool fromUp = false) override
        {
            return nullptr;
        }

        void setProgress(int current, int max);
    private:
        float progressPercentage = 0.0f;
        Label* label;
        ProgressSpinner* spinner;
};
