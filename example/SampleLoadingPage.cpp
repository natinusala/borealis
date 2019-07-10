/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
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

#include <SampleLoadingPage.hpp>

#include <math.h>

SampleLoadingPage::SampleLoadingPage(StagedAppletFrame *frame) : frame(frame)
{
    // Label
    this->progressDisp = new ProgressDisplay();
    this->progressDisp->setProgress(this->progressValue, 1000);
    this->label = new Label(LabelStyle::BUTTON, "Example loading display", true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);
}

void SampleLoadingPage::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    if (progressValue == 500)
        this->frame->nextStage();

    this->progressValue++;
    this->progressDisp->setProgress(this->progressValue, 500);
    this->progressDisp->frame(ctx);
    this->label->frame(ctx);
}

View* SampleLoadingPage::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    return nullptr;
}

void SampleLoadingPage::layout(NVGcontext* vg, Style *style, FontStash *stash)
{
    this->label->setWidth(roundf((float)this->width * style->CrashFrame.labelWidth));
    this->label->layout(vg, style, stash);

    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        this->y + (this->height - style->AppletFrame.footerHeight) / 2,
        this->label->getWidth(),
        this->label->getHeight()
    );

    this->progressDisp->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth,
        this->y + this->height / 2,
        style->CrashFrame.buttonWidth * 2,
        style->CrashFrame.buttonHeight
    );
}
