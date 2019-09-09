/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam

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

#include <borealis/applet_frame.hpp>
#include <borealis/application.hpp>

namespace brls
{

AppletFrame::AppletFrame(bool padLeft, bool padRight)
{
    Style* style = Application::getStyle();

    if (padLeft)
        this->leftPadding = style->AppletFrame.separatorSpacing;

    if (padRight)
        this->rightPadding = style->AppletFrame.separatorSpacing;
}

void AppletFrame::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    // Text
    if (this->headerStyle == HeaderStyle::REGULAR)
    {
        // Title
        NVGcolor titleColor = a(ctx->theme->textColor);

        if (this->contentView)
            titleColor.a *= this->contentView->getAlpha();

        nvgFillColor(vg, titleColor);
        nvgFontSize(vg, style->AppletFrame.titleSize);
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgBeginPath(vg);
        nvgText(vg, x + style->AppletFrame.titleStart, y + style->AppletFrame.headerHeightRegular / 2 + style->AppletFrame.titleOffset, this->title.c_str(), nullptr);

        // Header
        nvgBeginPath(vg);
        nvgFillColor(vg, a(ctx->theme->textColor));
        nvgRect(vg, x + style->AppletFrame.separatorSpacing, y + style->AppletFrame.headerHeightRegular - 1, width - style->AppletFrame.separatorSpacing * 2, 1);
        nvgFill(vg);
    }
    else if (this->headerStyle == HeaderStyle::POPUP)
    {
        // Header Text
        nvgBeginPath(vg);
        nvgFillColor(vg, a(ctx->theme->textColor));
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgFontSize(vg, style->PopupFrame.headerFontSize);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgText(vg, x + style->PopupFrame.headerTextLeftPadding,
            y + style->PopupFrame.headerTextTopPadding,
            this->title.c_str(),
            nullptr);

        // Sub title text 1
        nvgBeginPath(vg);
        nvgFillColor(vg, a(ctx->theme->descriptionColor));
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgFontSize(vg, style->PopupFrame.subTitleFontSize);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgText(vg, x + style->PopupFrame.subTitleLeftPadding,
            y + style->PopupFrame.subTitleTopPadding,
            this->subTitleLeft.c_str(),
            nullptr);

        float bounds[4];
        nvgTextBounds(vg, x, y, this->subTitleLeft.c_str(), nullptr, bounds);

        // Sub title separator
        nvgFillColor(vg, a(ctx->theme->descriptionColor)); // we purposely don't apply opacity
        nvgBeginPath(vg);
        nvgRect(vg, x + style->PopupFrame.subTitleLeftPadding + (bounds[2] - bounds[0]) + style->PopupFrame.subTitleSpacing,
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
        nvgText(vg, x + style->PopupFrame.subTitleLeftPadding + (bounds[2] - bounds[0]) + (style->PopupFrame.subTitleSpacing * 2),
            y + style->PopupFrame.subTitleTopPadding,
            this->subTitleRight.c_str(),
            nullptr);

        // Header
        nvgBeginPath(vg);
        nvgRect(vg, x + style->AppletFrame.separatorSpacing, y + style->AppletFrame.headerHeightPopup - 1, width - style->AppletFrame.separatorSpacing * 2, 1);
        nvgFill(vg);
    }

    // Footer
    NVGcolor footerColor = a(ctx->theme->textColor);

    if (this->slideIn)
        footerColor.a = 0.0f;
    else if (this->slideOut)
        footerColor.a = 1.0f;

    nvgFillColor(vg, footerColor);

    std::string* text = &this->footerText;

    if (*text == "")
        text = Application::getCommonFooter();

    nvgFontSize(vg, style->AppletFrame.footerTextSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgBeginPath(vg);
    nvgText(vg, x + style->AppletFrame.separatorSpacing + style->AppletFrame.footerTextSpacing, y + height - style->AppletFrame.footerHeight / 2, text->c_str(), nullptr);

    // Hint
    this->drawHint(ctx, x, y, width, height, footerColor);

    // Icon
    if (this->icon)
        this->icon->frame(ctx);

    // Separators
    nvgFillColor(vg, a(ctx->theme->separatorColor));

    // Footer
    nvgBeginPath(vg);
    nvgRect(vg, x + style->AppletFrame.separatorSpacing, y + height - style->AppletFrame.footerHeight, width - style->AppletFrame.separatorSpacing * 2, 1);
    nvgFill(vg);

    // Content view
    if (contentView)
    {
        float slideAlpha = 1.0f - this->contentView->alpha;

        if ((this->slideIn && this->animation == ViewAnimation::SLIDE_LEFT) || (this->slideOut && this->animation == ViewAnimation::SLIDE_RIGHT))
            slideAlpha = 1.0f - slideAlpha;

        int translation = (int)((float)style->AppletFrame.slideAnimation * slideAlpha);

        if ((this->slideIn && this->animation == ViewAnimation::SLIDE_LEFT) || (this->slideOut && this->animation == ViewAnimation::SLIDE_RIGHT))
            translation -= style->AppletFrame.slideAnimation;

        if (this->slideOut || this->slideIn)
            nvgTranslate(vg, -translation, 0);

        contentView->frame(ctx);

        if (this->slideOut || this->slideIn)
            nvgTranslate(vg, translation, 0);
    }
}

View* AppletFrame::requestFocus(FocusDirection direction, View* oldFocus, bool fromUp)
{
    if (fromUp)
        return View::requestFocus(direction, oldFocus);
    else if (this->contentView)
        return this->contentView->requestFocus(direction, oldFocus);
    return nullptr;
}

void AppletFrame::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    if (this->icon)
    {
        if (this->headerStyle == HeaderStyle::REGULAR)
        {
            // TODO: Icon
        }
        else if (this->headerStyle == HeaderStyle::POPUP)
        {
            this->icon->setBoundaries(style->PopupFrame.edgePadding + style->PopupFrame.imageLeftPadding, style->PopupFrame.imageTopPadding, style->PopupFrame.imageSize, style->PopupFrame.imageSize);
            this->icon->invalidate();
        }
    }

    if (this->contentView)
    {
        if (this->headerStyle == HeaderStyle::REGULAR)
            this->contentView->setBoundaries(this->x + leftPadding, this->y + style->AppletFrame.headerHeightRegular, this->width - this->leftPadding - this->rightPadding, this->height - style->AppletFrame.footerHeight - style->AppletFrame.headerHeightRegular);
        else if (this->headerStyle == HeaderStyle::POPUP)
            this->contentView->setBoundaries(this->x + leftPadding, this->y + style->AppletFrame.headerHeightPopup, this->width - this->leftPadding - this->rightPadding, this->height - style->AppletFrame.footerHeight - style->AppletFrame.headerHeightPopup);

        this->contentView->invalidate();
    }
}

void AppletFrame::setContentView(View* view)
{
    this->contentView = view;
    this->contentView->setParent(this);
    this->contentView->willAppear();
    this->invalidate();
}

void AppletFrame::setTitle(std::string title)
{
    this->title = title;
}

void AppletFrame::setFooterText(std::string footerText)
{
    this->footerText = footerText;
}

void AppletFrame::setSubtitle(std::string left, std::string right)
{
    this->subTitleLeft  = left;
    this->subTitleRight = right;
}

void AppletFrame::setIcon(unsigned char* buffer, size_t bufferSize)
{
    if (!this->icon)
    {
        this->icon = new Image(buffer, bufferSize);
        this->icon->setImageScaleType(ImageScaleType::SCALE);
        this->icon->setParent(this);
    }
    else
    {
        this->icon->setImage(buffer, bufferSize);
    }

    this->icon->invalidate();
}

void AppletFrame::setIcon(std::string imagePath)
{
    if (!this->icon)
    {
        this->icon = new Image(imagePath);
        this->icon->setImageScaleType(ImageScaleType::SCALE);
        this->icon->setParent(this);
    }
    else
    {
        this->icon->setImage(imagePath);
    }

    this->icon->invalidate();
}

void AppletFrame::setHeaderStyle(HeaderStyle headerStyle)
{
    this->headerStyle = headerStyle;

    this->invalidate();
}

AppletFrame::~AppletFrame()
{
    if (this->contentView)
    {
        this->contentView->willDisappear();
        delete this->contentView;
    }

    if (this->icon)
        delete this->icon;
}

void AppletFrame::willAppear()
{
    if (this->icon)
        this->icon->willAppear();

    if (this->contentView)
        this->contentView->willAppear();
}

void AppletFrame::willDisappear()
{
    if (this->icon)
        this->icon->willDisappear();

    if (this->contentView)
        this->contentView->willDisappear();
}

void AppletFrame::show(std::function<void(void)> cb, bool animated, ViewAnimation animation)
{
    this->animation = animation;

    if (animated && (animation == ViewAnimation::SLIDE_LEFT || animation == ViewAnimation::SLIDE_RIGHT) && this->contentView)
    {
        this->slideIn = true;

        this->contentView->show([this]() {
            this->slideIn = false;
        },
            true, animation);
    }
    else if (this->contentView->isHidden() && this->contentView)
    {
        this->contentView->show([]() {}, animated, animation);
    }

    View::show(cb, animated, animation);
}

void AppletFrame::hide(std::function<void(void)> cb, bool animated, ViewAnimation animation)
{
    this->animation = animation;

    if (animated && (animation == ViewAnimation::SLIDE_LEFT || animation == ViewAnimation::SLIDE_RIGHT) && this->contentView)
    {
        this->slideOut = true;

        this->contentView->hide([this, cb]() {
            this->slideOut = false;
        },
            true, animation);
    }
    else if (!this->contentView->isHidden() && this->contentView)
    {
        this->contentView->hide([]() {}, animated, animation);
    }

    View::hide(cb, animated, animation);
}

} // namespace brls
