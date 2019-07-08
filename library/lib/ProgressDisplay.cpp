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

#include <ProgressDisplay.hpp>

#include <Application.hpp>

ProgressDisplay::ProgressDisplay(unsigned progressStyle)
{
    if (progressStyle & PROGRESS_DISPLAY_STYLE_PERCENTAGE)
        this->label = new Label(LabelStyle::BUTTON, "0%", false);
    if (progressStyle & PROGRESS_DISPLAY_STYLE_SPINNER)
        this->spinner = new ProgressSpinner();
}

void ProgressDisplay::setProgress(int current, int max)
{
    if (current > max)
        return;

    this->progressPercentage = ((current * 100) / max);

    if (!this->label)
        return;

    string labelText = std::to_string((unsigned)this->progressPercentage);
    labelText += "%";
    this->label->setText(labelText);
}

void ProgressDisplay::layout(NVGcontext* vg, Style *style, FontStash *stash)
{
    if (this->label) {
        this->label->setWidth(style->ProgressDisplay.percentageLabelWidth);
        this->label->layout(vg, style, stash);
        this->label->setBoundaries(
            this->x + this->width - this->label->getWidth() / 2,
            this->y + this->height / 2 - this->label->getHeight() / 2,
            this->label->getWidth(),
            this->label->getHeight());
    }

    if (this->spinner) {
        this->spinner->setWidth(this->height);
        this->spinner->setHeight(this->height);
        this->spinner->setBoundaries(
            this->x,
            this->y,
            this->spinner->getWidth(),
            this->spinner->getHeight());
    }
}

void ProgressDisplay::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    unsigned progressBarWidth = width;
    unsigned progressBarX = x;

    if (this->label) {
        progressBarWidth -= this->label->getWidth();
        this->label->frame(ctx);
    }

    if (this->spinner) {
        // 1.25 accounts for the extra width of the curve on sides of progress bar
        progressBarWidth -= this->spinner->getWidth() * 1.25f;
        progressBarX += this->spinner->getWidth() * 1.25f;
        this->spinner->frame(ctx);
    }

    nvgBeginPath(vg);
    nvgMoveTo(vg, progressBarX, y + height / 2);
    nvgLineTo(vg, progressBarX + progressBarWidth, y + height / 2);
    nvgStrokeColor(vg, a(ctx->theme->listItemSeparatorColor));
    nvgStrokeWidth(vg, height / 3);
    nvgLineCap(vg, NVG_ROUND);
    nvgStroke(vg);

    if (this->progressPercentage > 0.0f) {
        nvgBeginPath(vg);
        nvgMoveTo(vg, progressBarX, y + height / 2);
        nvgLineTo(vg, progressBarX + ((float)progressBarWidth * this->progressPercentage) / 100, y + height / 2);
        nvgStrokeColor(vg, a(ctx->theme->listItemValueColor));
        nvgStrokeWidth(vg, height / 3);
        nvgLineCap(vg, NVG_ROUND);
        nvgStroke(vg);
    }
}

void ProgressDisplay::willAppear()
{
    if (this->spinner)
        this->spinner->willAppear();
}

void ProgressDisplay::willDisappear()
{
    if (this->spinner)
        this->spinner->willDisappear();
}
