/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

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

#include <AppletFrame.hpp>
#include <Application.hpp>

// TODO: Add hints system

AppletFrame::AppletFrame(bool padLeft, bool padRight)
{
    Style *style = Application::getStyle();

    if (padLeft)
        this->leftPadding = style->AppletFrame.separatorSpacing;

    if (padRight)
        this->rightPadding = style->AppletFrame.separatorSpacing;
}

void AppletFrame::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    // Text
    // Title
    nvgFillColor(vg, a(ctx->theme->textColor));
    nvgFontSize(vg, style->AppletFrame.titleSize);
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgBeginPath(vg);
    nvgText(vg, x + style->AppletFrame.titleStart, y + style->AppletFrame.headerHeight / 2 + style->AppletFrame.titleOffset, this->title.c_str(), nullptr);

    // Footer
    string *text = &this->subtitle;

    if (*text == "")
        text = Application::getCommonFooter();

    nvgFontSize(vg, style->AppletFrame.footerTextSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgBeginPath(vg);
    nvgText(vg, x + style->AppletFrame.separatorSpacing + style->AppletFrame.footerTextSpacing, y + height - style->AppletFrame.footerHeight/2, text->c_str(), nullptr);

    //TODO: Hint

    // TODO: Icon

    // Separators
    nvgFillColor(vg, a(ctx->theme->separatorColor));

    // Header
    nvgBeginPath(vg);
    nvgRect(vg, x + style->AppletFrame.separatorSpacing, y + style->AppletFrame.headerHeight - 1, width - style->AppletFrame.separatorSpacing * 2, 1);
    nvgFill(vg);

    // Footer
    nvgBeginPath(vg);
    nvgRect(vg, x + style->AppletFrame.separatorSpacing, y + height - style->AppletFrame.footerHeight, width - style->AppletFrame.separatorSpacing * 2, 1);
    nvgFill(vg);

    // Content view
    if (contentView)
        contentView->frame(ctx);
}

View* AppletFrame::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    if (fromUp)
        return View::requestFocus(direction, oldFocus);
    else if (this->contentView)
        return this->contentView->requestFocus(direction, oldFocus);
    return nullptr;
}

void AppletFrame::layout(NVGcontext* vg, Style *style, FontStash *stash)
{
    if (this->contentView)
    {
        this->contentView->setBoundaries(this->x + leftPadding, this->y + style->AppletFrame.headerHeight, this->width - this->leftPadding - this->rightPadding, this->height - style->AppletFrame.footerHeight - style->AppletFrame.headerHeight);
        this->contentView->invalidate();
    }
}

void AppletFrame::setContentView(View *view)
{
    this->contentView = view;
    this->contentView->setParent(this);
    this->contentView->willAppear();
    this->invalidate();
}

void AppletFrame::setTitle(string title)
{
    this->title = title;
}

void AppletFrame::setSubtitle(string subtitle)
{
    this->subtitle = subtitle;
}

AppletFrame::~AppletFrame()
{
    if (this->contentView)
    {
        this->contentView->willDisappear();
        delete this->contentView;
    }
}

void AppletFrame::willAppear()
{
    if (this->contentView)
        this->contentView->willAppear();
}

void AppletFrame::willDisappear()
{
    if (this->contentView)
        this->contentView->willDisappear();
}
