/*
    Copyright 2019-2021 natinusala
    Copyright 2019 p-sam

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <borealis/core/application.hpp>
#include <borealis/core/font.hpp>
#include <borealis/core/util.hpp>
#include <borealis/views/label.hpp>

namespace brls
{

#define ELLIPSIS "\u2026"

static void computeLabelHeight(Label* label, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode, YGSize* size, float* originalBounds)
{
    label->setIsWrapping(false);

    float requiredHeight = originalBounds[3] - originalBounds[1] + 5;
    if (heightMode == YGMeasureModeUndefined || heightMode == YGMeasureModeAtMost)
    {
        // Grow the label vertically as much as possible
        if (heightMode == YGMeasureModeAtMost)
            size->height = std::min(requiredHeight, height);
        else
            size->height = requiredHeight;
    }
    else if (heightMode == YGMeasureModeExactly)
    {
        size->height = height;
    }
    else
    {
        fatal("Unsupported Label height measure mode: " + std::to_string(heightMode));
    }
}

static YGSize labelMeasureFunc(YGNodeRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode)
{
    NVGcontext* vg       = Application::getNVGContext();
    Label* label         = (Label*)YGNodeGetContext(node);
    std::string fullText = label->getFullText();

    YGSize size = {
        width : width,
        height : height,
    };

    if (fullText == "")
        return size;

    // XXX: workaround for a Yoga bug
    if (widthMode == YGMeasureModeAtMost && (width == 0 || std::isnan(width)))
    {
        widthMode = YGMeasureModeUndefined;
        width     = NAN;
    }

    // Setup nvg state for the measurements
    nvgFontSize(vg, label->getFontSize());
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFontFaceId(vg, label->getFont());
    nvgTextLineHeight(vg, label->getLineHeight());

    // Measure the needed width for the ellipsis
    float bounds[4]; // width = xmax - xmin + some padding because nvgTextBounds isn't super precise
    nvgTextBounds(vg, 0, 0, ELLIPSIS, nullptr, bounds);
    float ellipsisWidth = bounds[2] - bounds[0] + 5;
    label->setEllipsisWidth(ellipsisWidth);

    // Measure the needed width for the fullText
    nvgTextBounds(vg, 0, 0, fullText.c_str(), nullptr, bounds);
    float requiredWidth = bounds[2] - bounds[0] + 5;
    label->setRequiredWidth(requiredWidth);

    // XXX: This is an approximation since the given width here may not match the actual final width of the view
    float availableWidth = std::isnan(width) ? std::numeric_limits<float>::max() : width;

    // Width
    if (widthMode == YGMeasureModeUndefined || widthMode == YGMeasureModeAtMost)
    {
        // Grow the label horizontally as much as possible
        if (widthMode == YGMeasureModeAtMost)
            size.width = std::min(requiredWidth, availableWidth);
        else
            size.width = requiredWidth;
    }
    else if (widthMode == YGMeasureModeExactly)
    {
        size.width = width;
    }
    else
    {
        fatal("Unsupported Label width measure mode: " + std::to_string(widthMode));
    }

    // Height
    // Measure the required height, with wrapping

    // Is wrapping necessary and allowed ?
    if (availableWidth < requiredWidth && !label->isSingleLine())
    {
        float boxBounds[4];
        nvgTextBoxBounds(vg, 0, 0, availableWidth, fullText.c_str(), nullptr, boxBounds);

        float requiredHeight = boxBounds[3] - boxBounds[1];

        // Undefined height mode, always wrap
        if (heightMode == YGMeasureModeUndefined)
        {
            label->setIsWrapping(true);
            size.height = requiredHeight;
        }
        // At most height mode, see if we have enough space
        else if (heightMode == YGMeasureModeAtMost)
        {
            if (height >= requiredHeight)
            {
                label->setIsWrapping(true);
                size.height = requiredHeight;
            }
            else
            {
                computeLabelHeight(label, width, widthMode, height, heightMode, &size, bounds);
            }
        }
        // Exactly mode, see if we have enough space
        else if (heightMode == YGMeasureModeExactly)
        {
            if (height >= requiredHeight)
            {
                label->setIsWrapping(true);
                size.height = height;
            }
            else
            {
                computeLabelHeight(label, width, widthMode, height, heightMode, &size, bounds);
            }
        }
        else
        {
            fatal("Unsupported Label height measure mode: " + std::to_string(heightMode));
        }
    }
    // No wrapping necessary or allowed, return the normal height
    else
    {
        computeLabelHeight(label, width, widthMode, height, heightMode, &size, bounds);
    }

    return size;
}

Label::Label()
{
    Style style = Application::getStyle();
    Theme theme = Application::getTheme();

    // Default attributes
    this->font       = Application::getFont(FONT_REGULAR);
    this->fontSize   = style["brls/label/default_font_size"];
    this->lineHeight = style["brls/label/default_line_height"];
    this->textColor  = theme["brls/text"];

    this->setHighlightPadding(style["brls/label/highlight_padding"]);

    // Setup the custom measure function
    YGNodeSetMeasureFunc(this->ygNode, labelMeasureFunc);

    // Set the max width and height to 100% to avoid overflowing
    // The view will be shortened if the text is too long
    YGNodeStyleSetMaxWidthPercent(this->ygNode, 100);
    YGNodeStyleSetMaxHeightPercent(this->ygNode, 100);

    // Register XML attributes
    this->registerStringXMLAttribute("text", [this](std::string value) {
        this->setText(value);
    });

    this->registerFloatXMLAttribute("fontSize", [this](float value) {
        this->setFontSize(value);
    });

    this->registerColorXMLAttribute("textColor", [this](NVGcolor color) {
        this->setTextColor(color);
    });

    this->registerFloatXMLAttribute("lineHeight", [this](float value) {
        this->setLineHeight(value);
    });

    this->registerBoolXMLAttribute("animated", [this](bool value) {
        this->setAnimated(value);
    });

    this->registerBoolXMLAttribute("autoAnimate", [this](bool value) {
        this->setAutoAnimate(value);
    });

    this->registerBoolXMLAttribute("singleLine", [this](bool value) {
        this->setSingleLine(value);
    });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "textAlign", TextAlign, this->setTextAlign,
        {
            { "left", TextAlign::LEFT },
            { "center", TextAlign::CENTER },
            { "right", TextAlign::RIGHT },
        });
}

void Label::setAnimated(bool animated)
{
    if (animated == this->animated || this->isWrapping || this->align != TextAlign::LEFT)
        return;

    this->animated = animated;

    this->resetScrollingAnimation();
}

void Label::setAutoAnimate(bool autoAnimate)
{
    this->autoAnimate = autoAnimate;
}

void Label::setTextAlign(TextAlign align)
{
    this->align = align;
}

void Label::onFocusGained()
{
    View::onFocusGained();

    if (this->autoAnimate)
        this->setAnimated(true);
}

void Label::onFocusLost()
{
    View::onFocusLost();

    if (this->autoAnimate)
        this->setAnimated(false);
}

void Label::onParentFocusGained(View* focusedView)
{
    View::onParentFocusGained(focusedView);

    if (this->autoAnimate)
        this->setAnimated(true);
}

void Label::onParentFocusLost(View* focusedView)
{
    View::onParentFocusLost(focusedView);

    if (this->autoAnimate)
        this->setAnimated(false);
}

void Label::setTextColor(NVGcolor color)
{
    this->textColor = color;
}

void Label::setText(std::string text)
{
    this->truncatedText = text;
    this->fullText      = text;

    this->invalidate();
}

void Label::setSingleLine(bool singleLine)
{
    this->singleLine = singleLine;

    this->invalidate();
}

void Label::setFontSize(float value)
{
    this->fontSize = value;

    this->invalidate();
}

void Label::setLineHeight(float value)
{
    this->lineHeight = value;

    this->invalidate();
}

void Label::setIsWrapping(bool isWrapping)
{
    this->isWrapping = isWrapping;
}

bool Label::isSingleLine()
{
    return this->singleLine;
}

static std::string trim(const std::string& str)
{
    std::string result = "";
    size_t endIndex    = str.size();
    while (endIndex > 0 && std::isblank(str[endIndex - 1]))
        endIndex -= 1;
    for (size_t i = 0; i < endIndex; i += 1)
    {
        char ch = str[i];
        if (!isblank(ch) || result.size() > 0)
            result += ch;
    }
    return result;
}

enum NVGalign Label::getNVGHorizontalAlign()
{
    switch (this->align)
    {
        default:
        case TextAlign::LEFT:
            return NVG_ALIGN_LEFT;
        case TextAlign::CENTER:
            return NVG_ALIGN_CENTER;
        case TextAlign::RIGHT:
            return NVG_ALIGN_RIGHT;
    }
}

void Label::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    if (width == 0)
        return;

    enum NVGalign horizAlign = this->getNVGHorizontalAlign();

    nvgFontSize(vg, this->fontSize);
    nvgTextAlign(vg, horizAlign | NVG_ALIGN_MIDDLE);
    nvgFontFaceId(vg, this->font);
    nvgTextLineHeight(vg, this->lineHeight);
    nvgFillColor(vg, a(this->textColor));

    // Animated text
    if (this->animating)
    {
        nvgSave(vg);
        nvgIntersectScissor(vg, x, y, width, height);

        float baseX   = x - this->scrollingAnimation;
        float spacing = style["brls/label/scrolling_animation_spacing"];

        nvgText(vg, baseX, y + height / 2.0f, this->fullText.c_str(), nullptr);

        if (this->scrollingAnimation > 0)
            nvgText(vg, baseX + this->requiredWidth + spacing, y + height / 2.0f, this->fullText.c_str(), nullptr);

        nvgRestore(vg);
    }
    // Wrapped text
    else if (this->isWrapping)
    {
        nvgTextAlign(vg, horizAlign | NVG_ALIGN_TOP);
        nvgTextBox(vg, x, y, width, this->fullText.c_str(), nullptr);
    }
    // Truncated text
    else
    {
        float textX = x;

        if (horizAlign == NVG_ALIGN_CENTER)
            textX += width / 2;
        else if (horizAlign == NVG_ALIGN_RIGHT)
            textX += width;

        nvgText(vg, textX, y + height / 2.0f, this->truncatedText.c_str(), nullptr);
    }
}

void Label::stopScrollingAnimation()
{
    // Extra check to avoid stopping inexisting timers on labels that
    // are never animated
    if (!this->animating)
        return;

    this->scrollingTimer.stop();
    this->scrollingTimer.reset();

    this->scrollingAnimation = 0.0f;

    this->animating = false;
}

void Label::onScrollTimerFinished()
{
    Style style = Application::getStyle();

    // Step 2: actual scrolling animation
    float target   = this->requiredWidth + style["brls/label/scrolling_animation_spacing"];
    float duration = target / style["brls/animations/label_scrolling_speed"];

    this->scrollingAnimation.reset();

    this->scrollingAnimation.addStep(target, duration, EasingFunction::linear);

    this->scrollingAnimation.setEndCallback([this](bool finished) {
        // Start over if the scrolling animation ended naturally
        if (finished)
            this->startScrollTimer();
    });

    this->scrollingAnimation.start();

    this->animating = true;
}

void Label::startScrollTimer()
{
    Style style = Application::getStyle();

    // Step 1: timer before starting to scroll
    this->scrollingAnimation = 0.0f;

    this->scrollingTimer.reset();

    this->scrollingTimer.setDuration(style["brls/animations/label_scrolling_timer"]);

    this->scrollingTimer.setEndCallback([this](bool finished) {
        this->onScrollTimerFinished();
    });

    this->scrollingTimer.start();

    this->animating = true;
}

void Label::resetScrollingAnimation()
{
    // Stop it
    this->stopScrollingAnimation();

    // Restart it if it needs to be
    if (this->animated)
    {
        float width = this->getWidth();

        if (width < this->requiredWidth)
            this->startScrollTimer();
    }
}

void Label::onLayout()
{
    float width = this->getWidth();

    if (width == 0)
    {
        this->resetScrollingAnimation();
        return;
    }

    // Prebake clipping
    if (width < this->requiredWidth && !this->isWrapping)
    {
        // Compute the position of the ellipsis (in chars), should the string be truncated
        // Use an approximation based on the text width and ellipsis width
        // Cannot do it in the measure function because the margins are not applied yet there
        float toRemove      = this->requiredWidth - width + this->ellipsisWidth * 1.5f; // little bit more than ellipsis width to make sure it doesn't overflow
        float toRemoveRatio = toRemove / requiredWidth;

        size_t ellipsisPosition = this->fullText.size() - roundf((float)this->fullText.size() * toRemoveRatio);
        this->truncatedText     = trim(this->fullText.substr(0, ellipsisPosition)) + ELLIPSIS;
    }
    else
    {
        this->truncatedText = this->fullText;
    }

    this->resetScrollingAnimation(); // either stops it or restarts it with the new text
}

int Label::getFont()
{
    return this->font;
}

float Label::getFontSize()
{
    return this->fontSize;
}

float Label::getLineHeight()
{
    return this->lineHeight;
}

std::string Label::getFullText()
{
    return this->fullText;
}

void Label::setRequiredWidth(float requiredWidth)
{
    this->requiredWidth = requiredWidth;
}

void Label::setEllipsisWidth(float ellipsisWidth)
{
    this->ellipsisWidth = ellipsisWidth;
}

Label::~Label()
{
    this->stopScrollingAnimation();
}

View* Label::create()
{
    return new Label();
}

} // namespace brls
