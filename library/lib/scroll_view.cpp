/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  natinusala

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

#include <borealis/scroll_view.hpp>

namespace brls
{

void ScrollView::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    if (this->contentView)
        this->contentView->frame(ctx);
}

void ScrollView::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    if (this->contentView)
    {
        this->contentView->setBoundaries(
            this->getX(),
            this->getY(),
            this->getWidth(),
            this->getHeight()
        );
        this->contentView->invalidate();
    }
}

void ScrollView::willAppear()
{
    if (this->contentView)
        this->contentView->willAppear();
}

void ScrollView::willDisappear()
{
    if (this->contentView)
        this->contentView->willDisappear();
}

View* ScrollView::getDefaultFocus()
{
    return this->contentView;
}

void ScrollView::setContentView(View* view)
{
    this->contentView = view;

    if (this->contentView)
    {
        this->contentView->setParent(this);
        this->contentView->willAppear();
    }

    this->invalidate();
}

View* ScrollView::getContentView()
{
    return this->contentView;
}

ScrollView::~ScrollView()
{
    if (this->contentView)
    {
         this->contentView->willDisappear();
         delete this->contentView;
    }
}

} // namespace brls
