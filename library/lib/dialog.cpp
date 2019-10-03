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

#include <borealis/dialog.hpp>
#include <borealis/application.hpp>

// TODO: different open animation?
// TODO: dynamic height (be careful: BoxLayout doesn't resize itself)

namespace brls
{

Dialog::Dialog(View* contentView) : contentView(contentView)
{
    if (contentView)
        contentView->setParent(this);
}

void Dialog::openWithView(View* contentView)
{
    Dialog* dialog = new Dialog(contentView);
    Application::pushView(dialog);
}

void Dialog::openWithText(std::string text)
{
    Label* label = new Label(LabelStyle::DIALOG, text, true);
    Dialog::openWithView(label);
}

void Dialog::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    //Backdrop
    nvgFillColor(vg, a(ctx->theme->dialogBackdrop));
    nvgBeginPath(vg);
    nvgRect(vg, x, y, width, height);
    nvgFill(vg);

    // Frame
    nvgFillColor(vg, a(ctx->theme->dialogColor));
    nvgBeginPath(vg);
    nvgRoundedRect(vg, this->frameX, this->frameY, this->frameWidth, this->frameHeight, style->Dialog.cornerRadius);
    nvgFill(vg);

    // Content view
    if (this->contentView)
        this->contentView->frame(ctx);
}

void Dialog::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    // TODO: change frameY and frameHeight if there are buttons

    this->frameWidth    = style->Dialog.width;
    this->frameHeight   = style->Dialog.height;

    this->frameX = getWidth()  / 2 - this->frameWidth  / 2;
    this->frameY = getHeight() / 2 - this->frameHeight / 2;

    unsigned contentX       = this->frameX + style->Dialog.paddingLeftRight;
    unsigned contentY       = this->frameY + style->Dialog.paddingTopBottom;
    unsigned contentWidth   = this->frameWidth - style->Dialog.paddingLeftRight * 2;
    unsigned contentHeight  = this->frameHeight - style->Dialog.paddingTopBottom * 2;

    if (this->contentView)
    {
        // First layout to get height
        this->contentView->setBoundaries(
            contentX,
            contentY,
            contentWidth,
            contentHeight
        );

        this->contentView->layout(vg, style, stash); // layout directly to get height

        // Center the content view in the dialog
        unsigned newContentHeight = this->contentView->getHeight();

        int difference = contentHeight - newContentHeight;

        if (difference < 0)
            difference = -difference;

        contentY += difference / 2;

        this->contentView->setBoundaries(
            contentX,
            contentY,
            contentWidth,
            contentHeight
        );

        this->contentView->invalidate();
    }
}

Dialog::~Dialog()
{
    if (this->contentView)
        delete this->contentView;
}

} // namespace brls

