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

#include <StagedAppletFrame.hpp>

#include <Style.hpp>
#include <Theme.hpp>
#include <Application.hpp>
#include <Logger.hpp>

StagedAppletFrame::StagedAppletFrame() : AppletFrame(true, true) {}

void StagedAppletFrame::addStage(View *view)
{
    stageViews.push_back(view);

    if (!currentStage)
        enterStage(0, false);
}

void StagedAppletFrame::nextStage()
{
    enterStage((int)currentStage + 1, true);
}

void StagedAppletFrame::previousStage()
{
    enterStage((int)currentStage - 1, true);
}

void StagedAppletFrame::enterStage(int index, bool requestFocus)
{
    if ((size_t)index >= stageViews.size())
        return;

    currentStage = (size_t)index;

    this->setContentView(stageViews[currentStage]);

    if (requestFocus)
        Application::requestFocus(this, FocusDirection::NONE);
}

void StagedAppletFrame::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    AppletFrame::draw(vg, x, y, width, height, style, ctx);

    if (stageViews.empty())
        return;

    for (size_t i = 0; i < stageViews.size(); i++) {
        bool current = (i == (stageViews.size() - 1) - currentStage);
        NVGcolor borderColor = ctx->theme->listItemSeparatorColor;
        nvgBeginPath(vg);
        nvgCircle(vg, x + width - style->AppletFrame.separatorSpacing * (style->StagedAppletFrame.progressIndicatorSpacing + i),
                  y + style->AppletFrame.headerHeight / 2, current ? style->StagedAppletFrame.progressIndicatorRadiusSelected :
                  style->StagedAppletFrame.progressIndicatorRadiusUnselected);

        if (current) {
            borderColor = ctx->theme->listItemSeparatorColor;
            nvgFillColor(vg, a(ctx->theme->listItemValueColor));
            nvgFill(vg);
        } else if (i > (stageViews.size() - 1) - currentStage) {
            borderColor = ctx->theme->separatorColor;
            nvgFillColor(vg, a(ctx->theme->textColor));
            nvgFill(vg);
        }

        nvgStrokeColor(vg, a(borderColor));
        nvgStrokeWidth(vg, style->StagedAppletFrame.progressIndicatorBorderWidth);
        nvgStroke(vg);
    }
}

unsigned StagedAppletFrame::getStagesCount()
{
    return this->stageViews.size();
}

unsigned StagedAppletFrame::getCurrentStage()
{
    return this->currentStage;
}

unsigned StagedAppletFrame::isLastStage()
{
    return this->currentStage == this->stageViews.size() - 1;
}

StagedAppletFrame::~StagedAppletFrame()
{
    stageViews.clear();

    // Content view is freed by ~AppletFrame()
}
