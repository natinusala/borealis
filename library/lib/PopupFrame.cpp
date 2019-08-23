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

#include <PopupFrame.hpp>
#include <Application.hpp>

#include <Animations.hpp>

#include <Logger.hpp>


PopupFrame::PopupFrame(string title, unsigned char *imageBuffer, size_t imageBufferSize, AppletFrame *contentView, string subTitleLeft, string subTitleRight) : contentView(contentView)
{    
    if (this->contentView)
    {
        this->contentView->setParent(this);
        this->contentView->setHeaderStyle(HeaderStyle::POPUP);
        this->contentView->setTitle(title);
        this->contentView->setSubtitle(subTitleLeft, subTitleRight);
        this->contentView->setIcon(imageBuffer, imageBufferSize);
        this->contentView->invalidate();
    }
}

PopupFrame::PopupFrame(string title, string imagePath, AppletFrame *contentView, string subTitleLeft, string subTitleRight) : contentView(contentView)
{
    if (this->contentView)
    {
        this->contentView->setParent(this);
        this->contentView->setHeaderStyle(HeaderStyle::POPUP);
        this->contentView->setTitle(title);
        this->contentView->setSubtitle(subTitleLeft, subTitleRight);
        this->contentView->setIcon(imagePath);
        this->contentView->invalidate();
    }
}

PopupFrame::PopupFrame(string title, AppletFrame *contentView, string subTitleLeft, string subTitleRight) : contentView(contentView)
{
    if (this->contentView)
    {
        this->contentView->setParent(this);
        this->contentView->setHeaderStyle(HeaderStyle::POPUP);
        this->contentView->setTitle(title);
        this->contentView->setSubtitle(subTitleLeft, subTitleRight);
        this->contentView->invalidate();
    }
}

void PopupFrame::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    // Backdrop
    nvgFillColor(vg, a(ctx->theme->dropdownBackgroundColor));
    nvgBeginPath(vg);
    nvgRect(vg, 0, y, width, height);
    nvgFill(vg);

    // Background
    nvgFillColor(vg, a(ctx->theme->backgroundColorRGB));
    nvgBeginPath(vg);
    nvgRect(vg, style->PopupFrame.edgePadding, y, width - style->PopupFrame.edgePadding * 2, height);
    nvgFill(vg);

    // TODO: Shadow
    
    // Content view
    nvgSave(vg);
    nvgScissor(vg, style->PopupFrame.edgePadding, 0, style->PopupFrame.contentWidth, height);

    this->contentView->frame(ctx);

    nvgRestore(vg);
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
    this->contentView->setBoundaries(style->PopupFrame.edgePadding, 0, style->PopupFrame.contentWidth, this->getHeight());
    this->contentView->invalidate();
}

View* PopupFrame::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    if (direction == FocusDirection::NONE)
        if (this->contentView)
            return this->contentView->requestFocus(direction, oldFocus, fromUp);
            
    return nullptr;
}

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

void PopupFrame::open(string title, AppletFrame *contentView, string subTitleLeft, string subTitleRight)
{
    PopupFrame *popupFrame = new PopupFrame(title, contentView, subTitleLeft, subTitleRight);
    Application::pushView(popupFrame);
}

void PopupFrame::willAppear()
{
    this->contentView->willAppear();
}

void PopupFrame::willDisappear()
{    
    this->contentView->willAppear();
}

PopupFrame::~PopupFrame()
{

}
