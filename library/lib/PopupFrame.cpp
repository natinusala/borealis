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

#include <PopupFrame.hpp>
#include <Application.hpp>

#include <Animations.hpp>

#include <Logger.hpp>


PopupFrame::PopupFrame(string title, unsigned char *imageBuffer, size_t imageBufferSize, AppletFrame *contentView, string subTitleLeft, string subTitleRight)
    : title(title),
      subTitleLeft(subTitleLeft),
      subTitleRight(subTitleRight),
      contentView(contentView)
{
    this->image = new Image(imageBuffer, imageBufferSize);
    this->image->setImageScaleType(ImageScaleType::SCALE);
    this->image->setParent(this);
    this->image->invalidate();
    
    if (this->contentView)
    {
        this->contentView->setParent(this);
        this->contentView->setHeaderStyle(HeaderStyle::LARGE);
        this->contentView->invalidate();
    }

}

PopupFrame::PopupFrame(string title, string imagePath, AppletFrame *contentView, string subTitleLeft, string subTitleRight)
    : title(title),
      subTitleLeft(subTitleLeft),
      subTitleRight(subTitleRight),
      contentView(contentView)
{
    this->image = new Image(imagePath);
    this->image->setImageScaleType(ImageScaleType::SCALE);
    this->image->setParent(this);
    this->image->invalidate();
    
    if (this->contentView)
    {
        this->contentView->setParent(this);
        this->contentView->setHeaderStyle(HeaderStyle::LARGE);
        this->contentView->invalidate();
    }
}

void PopupFrame::show(function<void(void)> cb, bool animate)
{
    View::show(cb);
}

void PopupFrame::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    // Backdrop
    nvgFillColor(vg, a(ctx->theme->dropdownBackgroundColor));
    nvgBeginPath(vg);
    nvgRect(vg, 0, y, width, height);
    nvgFill(vg);

    // TODO: Shadow

    // Background
    nvgFillColor(vg, a(ctx->theme->sidebarColor));
    nvgBeginPath(vg);
    nvgRect(vg, x + style->PopupFrame.edgePadding, y, width - 2 * style->PopupFrame.edgePadding, height);
    nvgFill(vg);

    // Image
    this->image->frame(ctx);

    // Header Text
    nvgBeginPath(vg);
    nvgFillColor(vg, a(ctx->theme->textColor));
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgFontSize(vg, style->PopupFrame.headerFontSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + style->PopupFrame.edgePadding + style->PopupFrame.headerTextLeftPadding,
        y + style->PopupFrame.headerTextTopPadding,
        this->title.c_str(),
        nullptr);

    // Sub title text 1
    nvgBeginPath(vg);
    nvgFillColor(vg, a(ctx->theme->descriptionColor));
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgFontSize(vg, style->PopupFrame.subTitleFontSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(vg, x + style->PopupFrame.edgePadding + style->PopupFrame.subTitleLeftPadding,
        y + style->PopupFrame.subTitleTopPadding,
        this->subTitleLeft.c_str(),
        nullptr);

    float bounds[4];
    nvgTextBounds(vg, x, y, this->subTitleLeft.c_str(), nullptr, bounds);

    // Sub title separator
    nvgFillColor(vg, a(ctx->theme->descriptionColor)); // we purposely don't apply opacity
    nvgBeginPath(vg);
    nvgRect(vg, x + style->PopupFrame.edgePadding + style->PopupFrame.subTitleLeftPadding + (bounds[2] - bounds[0]) + style->PopupFrame.subTitleSpacing,
        y + style->PopupFrame.subTitleSeparatorTopPadding,
        1,
        style->PopupFrame.subTitleSeparatorHeight);
    nvgFill(vg);

    // Sub title text 2
    nvgBeginPath(vg);
    nvgFillColor(vg, a(ctx->theme->descriptionColor));
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgFontSize(vg, style->PopupFrame.subTitleFontSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(vg, x + style->PopupFrame.edgePadding + style->PopupFrame.subTitleLeftPadding + (bounds[2] - bounds[0]) + (style->PopupFrame.subTitleSpacing * 2),
        y + style->PopupFrame.subTitleTopPadding,
        this->subTitleRight.c_str(),
        nullptr);
    
    // Content view
    if (this->contentView)
    {
        nvgSave(vg);
        nvgScissor(vg, style->PopupFrame.edgePadding, 0, style->PopupFrame.contentWidth, height);

        this->contentView->frame(ctx);

        nvgRestore(vg);
    }
}

bool PopupFrame::onCancel()
{
    Application::popView();
    return true;
}

unsigned PopupFrame::getShowAnimationDuration()
{
    return View::getShowAnimationDuration() / 2;
}

void PopupFrame::layout(NVGcontext* vg, Style *style, FontStash *stash)
{
    this->image->setBoundaries(style->PopupFrame.edgePadding + style->PopupFrame.imageLeftPadding, style->PopupFrame.imageTopPadding, style->PopupFrame.imageSize, style->PopupFrame.imageSize);
    this->image->invalidate();

    if (this->contentView)
    {
        this->contentView->setBoundaries(style->PopupFrame.edgePadding, 0, style->PopupFrame.contentWidth, 720);
        this->contentView->invalidate();
    }
}

View* PopupFrame::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    if (direction == FocusDirection::NONE)
        if (this->contentView)
            return this->contentView->requestFocus(direction, oldFocus, fromUp);
            
    return nullptr;}

void PopupFrame::open(string title, unsigned char *imageBuffer, size_t imageBufferSize, AppletFrame *contentView, string subTitleLeft, string subTitleRight)
{
    PopupFrame *popupFrame = new PopupFrame(title, imageBuffer, imageBufferSize, contentView, subTitleLeft, subTitleRight);
    Application::pushView(popupFrame);
}

void PopupFrame::open(string title, string imagePath, AppletFrame *contentView, string subTitleLeft, string subTitleRight)
{
    PopupFrame *popupFrame = new PopupFrame(title, imagePath, contentView, subTitleLeft, subTitleRight);
    Application::pushView(popupFrame);
}

void PopupFrame::willAppear()
{
    this->image->willAppear();

    if (this->contentView)
        this->contentView->willAppear();
}

void PopupFrame::willDisappear()
{
    this->image->willDisappear();
    
    if (this->contentView)
        this->contentView->willAppear();
}

PopupFrame::~PopupFrame()
{

}
