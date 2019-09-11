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

#include <borealis/application.hpp>
#include <borealis/thumbnail_frame.hpp>

namespace brls
{

ThumbnailFrame::ThumbnailFrame()
    : AppletFrame(true, false)
{
    // Create the ThumbnailSidebar
    this->sidebar = new ThumbnailSidebar();

    // Setup content view
    this->boxLayout = new BoxLayout(BoxLayoutOrientation::HORIZONTAL);
    AppletFrame::setContentView(this->boxLayout);
}

void ThumbnailFrame::setContentView(View* view)
{
    this->thumbnailContentView = view;

    // Clear the layout
    this->boxLayout->clear();

    // Add the views
    this->boxLayout->addView(view);
    this->boxLayout->addView(this->sidebar);

    // Invalidate
    this->invalidate();
}

void ThumbnailFrame::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    // Resize content view
    if (this->thumbnailContentView)
    {
        unsigned sidebarWidth = this->sidebar->getWidth();
        this->thumbnailContentView->setWidth(this->getWidth() - sidebarWidth - this->leftPadding - this->rightPadding);
    }

    // Layout the rest
    AppletFrame::layout(vg, style, stash);
}

ThumbnailSidebar* ThumbnailFrame::getSidebar()
{
    return this->sidebar;
}

ThumbnailFrame::~ThumbnailFrame()
{
    // If content view wasn't set, free the sidebar manually
    if (!this->thumbnailContentView)
        delete this->sidebar;
}

ThumbnailSidebar::ThumbnailSidebar()
{
    Style* style = Application::getStyle();

    this->setBackground(Background::SIDEBAR);
    this->setWidth(style->Sidebar.width);
}

void ThumbnailSidebar::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    if (this->image)
        this->image->frame(ctx);
}

void ThumbnailSidebar::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    if (this->image)
    {
        unsigned size = getWidth() - style->ThumbnailSidebar.marginLeftRight * 2;
        this->image->setBoundaries(
            getX() + style->ThumbnailSidebar.marginLeftRight,
            getY() + style->ThumbnailSidebar.marginTopBottom,
            size,
            size
        );
    }
}

void ThumbnailSidebar::setThumbnail(std::string imagePath)
{
    if (this->image)
    {
        this->image->setImage(imagePath);
    }
    else
    {
        this->image = new Image(imagePath);
        this->image->setImageScaleType(ImageScaleType::FIT);
        this->image->setParent(this);
        this->invalidate();
    }
}

void ThumbnailSidebar::setThumbnail(unsigned char* buffer, size_t bufferSize)
{
    if (this->image)
    {
        this->image->setImage(buffer, bufferSize);
    }
    else
    {
        this->image = new Image(buffer, bufferSize);
        //this->image->setImageScaleType(ImageScaleType::FIT);
        this->image->setParent(this);
        this->invalidate();
    }
}

ThumbnailSidebar::~ThumbnailSidebar()
{
    if (this->image)
        delete this->image;
}

} // namespace brls
