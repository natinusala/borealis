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
        enterStage(0);
}

void StagedAppletFrame::nextStage()
{
    if (currentStage + 1 >= stageViews.size())
        return;

    currentStage++;
    this->setContentView(stageViews[currentStage]);
    Application::requestFocus(stageViews[currentStage], FocusDirection::NONE);
}

void StagedAppletFrame::previousStage()
{
    if (currentStage == 0)
        return;

    currentStage--;
    this->setContentView(stageViews[currentStage]);
    Application::requestFocus(stageViews[currentStage], FocusDirection::NONE);
}

void StagedAppletFrame::enterStage(int index)
{
    if ((size_t)index >= stageViews.size())
        return;

    currentStage = (size_t)index;

    this->setContentView(stageViews[currentStage]);
    Application::requestFocus(stageViews[currentStage], FocusDirection::NONE);
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
        nvgCircle(vg, x + width - style->AppletFrame.separatorSpacing * (4 + i), y + style->AppletFrame.headerHeight / 2, current ? 10 : 6);

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
        nvgStrokeWidth(vg, 2);
        nvgStroke(vg);
    }
}

StagedAppletFrame::~StagedAppletFrame()
{
    stageViews.clear();

    // Content view is freed by ~AppletFrame()
}
