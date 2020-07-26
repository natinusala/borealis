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

#include <borealis/absolute_layout.hpp>

namespace brls
{

void AbsoluteLayout::addView(View* view)
{
    view->setParent(this);

    this->children.push_back(view);
    this->invalidate();
}

void AbsoluteLayout::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    for (View* view : this->children)
        view->frame(ctx);
}

void AbsoluteLayout::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    for (View* view : this->children)
        view->invalidate();
}

void AbsoluteLayout::onWindowSizeChanged()
{
    for (View* view : this->children)
        view->onWindowSizeChanged();
}

void AbsoluteLayout::willAppear(bool resetState)
{
    for (View* view : this->children)
        view->willAppear(resetState);
}

void AbsoluteLayout::willDisappear(bool resetState)
{
    for (View* view : this->children)
        view->willDisappear(resetState);
}

AbsoluteLayout::~AbsoluteLayout()
{
    for (View* view : this->children)
        delete view;
}

} // namespace brls
