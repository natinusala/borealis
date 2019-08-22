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

// TODO: Add animation for bullets when changing stage
// TODO: Add left/right and right/left animations for views when changing stage - put the animation in AppletFrame::setContentView()
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

    for (size_t i = 0; i < stageViews.size(); i++)
    {
        bool current = (i == (stageViews.size() - 1) - currentStage);

        // Border color for next stages
        NVGcolor borderColor = ctx->theme->nextStageBulletColor;

        nvgBeginPath(vg);
        nvgCircle(vg, x + width - style->AppletFrame.separatorSpacing * (style->StagedAppletFrame.progressIndicatorSpacing + i) + style->AppletFrame.separatorSpacing + style->AppletFrame.separatorSpacing / 2,
                  y + style->AppletFrame.headerHeightRegular / 2, current ? style->StagedAppletFrame.progressIndicatorRadiusSelected :
                  style->StagedAppletFrame.progressIndicatorRadiusUnselected);

        // Current stage
        if (current)
        {
            nvgFillColor(vg, a(ctx->theme->listItemValueColor));
            nvgFill(vg);
        }
        // Previous stages
        else if (i > (stageViews.size() - 1) - currentStage)
        {
            borderColor = ctx->theme->separatorColor;
            nvgFillColor(vg, a(ctx->theme->textColor));
            nvgFill(vg);
        }

        if (!current)
        {
            nvgStrokeColor(vg, a(borderColor));
            nvgStrokeWidth(vg, style->StagedAppletFrame.progressIndicatorBorderWidth);
            nvgStroke(vg);
        }
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
