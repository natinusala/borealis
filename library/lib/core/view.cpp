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

#include <math.h>

#include <algorithm>
#include <borealis/core/animation.hpp>
#include <borealis/core/application.hpp>
#include <borealis/core/box.hpp>
#include <borealis/core/i18n.hpp>
#include <borealis/core/input.hpp>
#include <borealis/core/util.hpp>
#include <borealis/core/view.hpp>

using namespace brls::literals;

namespace brls
{

static bool endsWith(const std::string& data, const std::string& suffix)
{
    return data.find(suffix, data.size() - suffix.size()) != std::string::npos;
}

static bool startsWith(const std::string& data, const std::string& prefix)
{
    return data.rfind(prefix, 0) == 0;
}

View::View()
{
    // Instantiate and prepare YGNode
    this->ygNode = YGNodeNew();
    YGNodeSetContext(this->ygNode, this);

    YGNodeStyleSetWidthAuto(this->ygNode);
    YGNodeStyleSetHeightAuto(this->ygNode);

    // Register common XML attributes
    this->registerCommonAttributes();

    // Default values
    Style style = Application::getStyle();

    this->highlightCornerRadius = style["brls/highlight/corner_radius"];
}

static int shakeAnimation(float t, float a) // a = amplitude
{
    // Damped sine wave
    float w = 0.8f; // period
    float c = 0.35f; // damp factor
    return roundf(a * exp(-(c * t)) * sin(w * t));
}

void View::shakeHighlight(FocusDirection direction)
{
    this->highlightShaking        = true;
    this->highlightShakeStart     = getCPUTimeUsec() / 1000;
    this->highlightShakeDirection = direction;
    this->highlightShakeAmplitude = std::rand() % 15 + 10;
}

float View::getAlpha(bool child)
{
    return this->alpha * (this->parent ? this->parent->getAlpha(true) : 1.0f);
}

NVGcolor View::a(NVGcolor color)
{
    NVGcolor newColor = color; // copy
    newColor.a *= this->getAlpha();
    return newColor;
}

NVGpaint View::a(NVGpaint paint)
{
    NVGpaint newPaint = paint; // copy
    newPaint.innerColor.a *= this->getAlpha();
    newPaint.outerColor.a *= this->getAlpha();
    return newPaint;
}

void View::frame(FrameContext* ctx)
{
    if (this->visibility != Visibility::VISIBLE)
        return;

    Style style    = Application::getStyle();
    Theme oldTheme = ctx->theme;

    nvgSave(ctx->vg);

    // Theme override
    if (this->themeOverride)
        ctx->theme = *themeOverride;

    float x      = this->getX();
    float y      = this->getY();
    float width  = this->getWidth();
    float height = this->getHeight();

    if (this->alpha > 0.0f && this->collapseState != 0.0f)
    {
        // Draw background
        this->drawBackground(ctx->vg, ctx, style);

        // Draw shadow
        if (this->shadowType != ShadowType::NONE && this->showShadow)
            this->drawShadow(ctx->vg, ctx, style, x, y, width, height);

        // Draw border
        if (this->borderThickness > 0.0f)
            this->drawBorder(ctx->vg, ctx, style, x, y, width, height);

        // Draw highlight background
        if (this->highlightAlpha > 0.0f && !this->hideHighlightBackground)
            this->drawHighlight(ctx->vg, ctx->theme, this->highlightAlpha, style, true);

        // Draw click animation
        if (this->clickAlpha > 0.0f)
            this->drawClickAnimation(ctx->vg, ctx, x, y, width, height);

        // Collapse clipping
        if (this->collapseState < 1.0f)
        {
            nvgSave(ctx->vg);
            nvgIntersectScissor(ctx->vg, x, y, width, height * this->collapseState);
        }

        // Draw the view
        this->draw(ctx->vg, x, y, width, height, style, ctx);

        // Draw highlight
        if (this->highlightAlpha > 0.0f)
            this->drawHighlight(ctx->vg, ctx->theme, this->highlightAlpha, style, false);

        if (this->wireframeEnabled)
            this->drawWireframe(ctx, x, y, width, height);

        this->drawLine(ctx, x, y, width, height);

        //Reset clipping
        if (this->collapseState < 1.0f)
            nvgRestore(ctx->vg);
    }

    // Cleanup
    if (this->themeOverride)
        ctx->theme = oldTheme;

    nvgRestore(ctx->vg);
}

void View::resetClickAnimation()
{
    this->clickAlpha.stop();
}

void View::playClickAnimation(bool reverse)
{
    this->resetClickAnimation();

    Style style = Application::getStyle();

    this->clickAlpha.reset(reverse ? 1.0f : 0.0f);

    this->clickAlpha.addStep(
        reverse ? 0.0f : 1.0f,
        style["brls/animations/highlight"],
        reverse ? EasingFunction::quadraticOut : EasingFunction::quadraticIn);

    this->clickAlpha.setEndCallback([this, reverse](bool finished) {
        if (reverse)
            return;

        this->playClickAnimation(true);
    });

    this->clickAlpha.start();
}

void View::drawClickAnimation(NVGcontext* vg, FrameContext* ctx, float x, float y, float width, float height)
{
    Theme theme    = ctx->theme;
    NVGcolor color = theme["brls/click_pulse"];

    color.a *= this->clickAlpha;

    nvgFillColor(vg, a(color));
    nvgBeginPath(vg);

    if (this->cornerRadius > 0.0f)
        nvgRoundedRect(vg, x, y, width, height, this->cornerRadius);
    else
        nvgRect(vg, x, y, width, height);

    nvgFill(vg);
}

void View::drawLine(FrameContext* ctx, float x, float y, float width, float height)
{
    // Don't setup and draw empty nvg path if there is no line to draw
    if (this->lineTop <= 0 && this->lineRight <= 0 && this->lineBottom <= 0 && this->lineLeft <= 0)
        return;

    nvgBeginPath(ctx->vg);
    nvgFillColor(ctx->vg, a(this->lineColor));

    if (this->lineTop > 0)
        nvgRect(ctx->vg, x, y, width, this->lineTop);

    if (this->lineRight > 0)
        nvgRect(ctx->vg, x + width, y, this->lineRight, height);

    if (this->lineBottom > 0)
        nvgRect(ctx->vg, x, y + height - this->lineBottom, width, this->lineBottom);

    if (this->lineLeft > 0)
        nvgRect(ctx->vg, x - this->lineLeft, y, this->lineLeft, height);

    nvgFill(ctx->vg);
}

void View::drawWireframe(FrameContext* ctx, float x, float y, float width, float height)
{
    nvgStrokeWidth(ctx->vg, 1);

    // Outline
    nvgBeginPath(ctx->vg);
    nvgStrokeColor(ctx->vg, nvgRGB(0, 0, 255));
    nvgRect(ctx->vg, x, y, width, height);
    nvgStroke(ctx->vg);

    if (this->hasParent())
    {
        // Diagonals
        nvgFillColor(ctx->vg, nvgRGB(0, 0, 255));

        nvgBeginPath(ctx->vg);
        nvgMoveTo(ctx->vg, x, y);
        nvgLineTo(ctx->vg, x + width, y + height);
        nvgFill(ctx->vg);

        nvgBeginPath(ctx->vg);
        nvgMoveTo(ctx->vg, x + width, y);
        nvgLineTo(ctx->vg, x, y + height);
        nvgFill(ctx->vg);
    }

    // Padding
    nvgBeginPath(ctx->vg);
    nvgStrokeColor(ctx->vg, nvgRGB(0, 255, 0));

    float paddingTop    = ntz(YGNodeLayoutGetPadding(this->ygNode, YGEdgeTop));
    float paddingLeft   = ntz(YGNodeLayoutGetPadding(this->ygNode, YGEdgeLeft));
    float paddingBottom = ntz(YGNodeLayoutGetPadding(this->ygNode, YGEdgeBottom));
    float paddingRight  = ntz(YGNodeLayoutGetPadding(this->ygNode, YGEdgeRight));

    // Top
    if (paddingTop > 0)
        nvgRect(ctx->vg, x, y, width, paddingTop);

    // Right
    if (paddingRight > 0)
        nvgRect(ctx->vg, x + width - paddingRight, y, paddingRight, height);

    // Bottom
    if (paddingBottom > 0)
        nvgRect(ctx->vg, x, y + height - paddingBottom, width, paddingBottom);

    // Left
    if (paddingLeft > 0)
        nvgRect(ctx->vg, x, y, paddingLeft, height);

    nvgStroke(ctx->vg);

    // Margins
    nvgBeginPath(ctx->vg);
    nvgStrokeColor(ctx->vg, nvgRGB(255, 0, 0));

    float marginTop    = ntz(YGNodeLayoutGetMargin(this->ygNode, YGEdgeTop));
    float marginLeft   = ntz(YGNodeLayoutGetMargin(this->ygNode, YGEdgeLeft));
    float marginBottom = ntz(YGNodeLayoutGetMargin(this->ygNode, YGEdgeBottom));
    float marginRight  = ntz(YGNodeLayoutGetMargin(this->ygNode, YGEdgeRight));

    // Top
    if (marginTop > 0)
        nvgRect(ctx->vg, x - marginLeft, y - marginTop, width + marginLeft + marginRight, marginTop);

    // Right
    if (marginRight > 0)
        nvgRect(ctx->vg, x + width, y - marginTop, marginRight, height + marginTop + marginBottom);

    // Bottom
    if (marginBottom > 0)
        nvgRect(ctx->vg, x - marginLeft, y + height, width + marginLeft + marginRight, marginBottom);

    // Left
    if (marginLeft > 0)
        nvgRect(ctx->vg, x - marginLeft, y - marginTop, marginLeft, height + marginTop + marginBottom);

    nvgStroke(ctx->vg);
}

void View::drawBorder(NVGcontext* vg, FrameContext* ctx, Style style, float x, float y, float width, float height)
{
    nvgBeginPath(vg);
    nvgStrokeColor(vg, this->borderColor);
    nvgStrokeWidth(vg, this->borderThickness);
    nvgRoundedRect(vg, x, y, width, height, this->cornerRadius);
    nvgStroke(vg);
}

void View::drawShadow(NVGcontext* vg, FrameContext* ctx, Style style, float x, float y, float width, float height)
{
    float shadowWidth   = 0.0f;
    float shadowFeather = 0.0f;
    float shadowOpacity = 0.0f;
    float shadowOffset  = 0.0f;

    switch (this->shadowType)
    {
        case ShadowType::GENERIC:
            shadowWidth   = style["brls/shadow/width"];
            shadowFeather = style["brls/shadow/feather"];
            shadowOpacity = style["brls/shadow/opacity"];
            shadowOffset  = style["brls/shadow/offset"];
            break;
        case ShadowType::CUSTOM:
            break;
        case ShadowType::NONE:
            break;
    }

    NVGpaint shadowPaint = nvgBoxGradient(
        vg,
        x, y + shadowWidth,
        width, height,
        this->cornerRadius * 2, shadowFeather,
        RGBA(0, 0, 0, shadowOpacity * alpha), TRANSPARENT);

    nvgBeginPath(vg);
    nvgRect(
        vg,
        x - shadowOffset,
        y - shadowOffset,
        width + shadowOffset * 2,
        height + shadowOffset * 3);
    nvgRoundedRect(vg, x, y, width, height, this->cornerRadius);
    nvgPathWinding(vg, NVG_HOLE);
    nvgFillPaint(vg, shadowPaint);
    nvgFill(vg);
}

void View::collapse(bool animated)
{
    if (animated)
    {
        Style style = Application::getStyle();

        this->collapseState.reset();

        this->collapseState.addStep(0.0f, style["brls/animations/collapse"], EasingFunction::quadraticOut);

        this->collapseState.setTickCallback([this] {
            if (this->hasParent())
                this->getParent()->invalidate();
        });

        this->collapseState.start();
    }
    else
    {
        this->collapseState = 0.0f;
    }
}

bool View::isCollapsed()
{
    return this->collapseState < 1.0f;
}

void View::expand(bool animated)
{
    if (animated)
    {
        Style style = Application::getStyle();

        this->collapseState.reset();

        this->collapseState.addStep(1.0f, style["brls/animations/collapse"], EasingFunction::quadraticOut);

        this->collapseState.setTickCallback([this] {
            if (this->hasParent())
                this->getParent()->invalidate();
        });

        this->collapseState.start();
    }
    else
    {
        this->collapseState = 1.0f;
    }
}

void View::setAlpha(float alpha)
{
    this->alpha = alpha;
}

void View::drawHighlight(NVGcontext* vg, Theme theme, float alpha, Style style, bool background)
{
    nvgSave(vg);
    nvgResetScissor(vg);

    float padding      = this->highlightPadding;
    float cornerRadius = this->highlightCornerRadius;
    float strokeWidth  = style["brls/highlight/stroke_width"];

    float x      = this->getX() - padding - strokeWidth / 2;
    float y      = this->getY() - padding - strokeWidth / 2;
    float width  = this->getWidth() + padding * 2 + strokeWidth;
    float height = this->getHeight() + padding * 2 + strokeWidth;

    // Shake animation
    if (this->highlightShaking)
    {
        Time curTime = getCPUTimeUsec() / 1000;
        Time t       = (curTime - highlightShakeStart) / 10;

        if (t >= style["brls/animations/highlight_shake"])
        {
            this->highlightShaking = false;
        }
        else
        {
            switch (this->highlightShakeDirection)
            {
                case FocusDirection::RIGHT:
                    x += shakeAnimation(t, this->highlightShakeAmplitude);
                    break;
                case FocusDirection::LEFT:
                    x -= shakeAnimation(t, this->highlightShakeAmplitude);
                    break;
                case FocusDirection::DOWN:
                    y += shakeAnimation(t, this->highlightShakeAmplitude);
                    break;
                case FocusDirection::UP:
                    y -= shakeAnimation(t, this->highlightShakeAmplitude);
                    break;
            }
        }
    }

    // Draw
    if (background)
    {
        // Background
        NVGcolor highlightBackgroundColor = theme["brls/highlight/background"];
        nvgFillColor(vg, RGBAf(highlightBackgroundColor.r, highlightBackgroundColor.g, highlightBackgroundColor.b, this->highlightAlpha));
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgFill(vg);
    }
    else
    {
        float shadowOffset = style["brls/highlight/shadow_offset"];

        // Shadow
        NVGpaint shadowPaint = nvgBoxGradient(vg,
            x, y + style["brls/highlight/shadow_width"],
            width, height,
            cornerRadius * 2, style["brls/highlight/shadow_feather"],
            RGBA(0, 0, 0, style["brls/highlight/shadow_opacity"] * alpha), TRANSPARENT);

        nvgBeginPath(vg);
        nvgRect(vg, x - shadowOffset, y - shadowOffset,
            width + shadowOffset * 2, height + shadowOffset * 3);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgPathWinding(vg, NVG_HOLE);
        nvgFillPaint(vg, shadowPaint);
        nvgFill(vg);

        // Border
        float gradientX, gradientY, color;
        getHighlightAnimation(&gradientX, &gradientY, &color);

        NVGcolor highlightColor1 = theme["brls/highlight/color1"];

        NVGcolor pulsationColor = RGBAf((color * highlightColor1.r) + (1 - color) * highlightColor1.r,
            (color * highlightColor1.g) + (1 - color) * highlightColor1.g,
            (color * highlightColor1.b) + (1 - color) * highlightColor1.b,
            alpha);

        NVGcolor borderColor = theme["brls/highlight/color2"];
        borderColor.a        = 0.5f * alpha * this->getAlpha();

        float strokeWidth = style["brls/highlight/stroke_width"];

        NVGpaint border1Paint = nvgRadialGradient(vg,
            x + gradientX * width, y + gradientY * height,
            strokeWidth * 10, strokeWidth * 40,
            borderColor, TRANSPARENT);

        NVGpaint border2Paint = nvgRadialGradient(vg,
            x + (1 - gradientX) * width, y + (1 - gradientY) * height,
            strokeWidth * 10, strokeWidth * 40,
            borderColor, TRANSPARENT);

        nvgBeginPath(vg);
        nvgStrokeColor(vg, pulsationColor);
        nvgStrokeWidth(vg, strokeWidth);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgStrokePaint(vg, border1Paint);
        nvgStrokeWidth(vg, strokeWidth);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgStrokePaint(vg, border2Paint);
        nvgStrokeWidth(vg, strokeWidth);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgStroke(vg);
    }

    nvgRestore(vg);
}

void View::setBackground(ViewBackground background)
{
    this->background = background;
}

void View::drawBackground(NVGcontext* vg, FrameContext* ctx, Style style)
{
    float x      = this->getX();
    float y      = this->getY();
    float width  = this->getWidth();
    float height = this->getHeight();

    Theme theme = ctx->theme;

    switch (this->background)
    {
        case ViewBackground::SIDEBAR:
        {
            float backdropHeight  = style["brls/sidebar/border_height"];
            NVGcolor sidebarColor = theme["brls/sidebar/background"];

            // Solid color
            nvgBeginPath(vg);
            nvgFillColor(vg, a(sidebarColor));
            nvgRect(vg, x, y + backdropHeight, width, height - backdropHeight * 2);
            nvgFill(vg);

            //Borders gradient
            // Top
            NVGpaint topGradient = nvgLinearGradient(vg, x, y + backdropHeight, x, y, a(sidebarColor), TRANSPARENT);
            nvgBeginPath(vg);
            nvgFillPaint(vg, topGradient);
            nvgRect(vg, x, y, width, backdropHeight);
            nvgFill(vg);

            // Bottom
            NVGpaint bottomGradient = nvgLinearGradient(vg, x, y + height - backdropHeight, x, y + height, a(sidebarColor), TRANSPARENT);
            nvgBeginPath(vg);
            nvgFillPaint(vg, bottomGradient);
            nvgRect(vg, x, y + height - backdropHeight, width, backdropHeight);
            nvgFill(vg);
            break;
        }
        case ViewBackground::BACKDROP:
        {
            nvgFillColor(vg, a(theme["brls/backdrop"]));
            nvgBeginPath(vg);
            nvgRect(vg, x, y, width, height);
            nvgFill(vg);
        }
        case ViewBackground::SHAPE_COLOR:
        {
            nvgFillColor(vg, a(this->backgroundColor));
            nvgBeginPath(vg);

            if (this->cornerRadius > 0.0f)
                nvgRoundedRect(vg, x, y, width, height, this->cornerRadius);
            else
                nvgRect(vg, x, y, width, height);

            nvgFill(vg);
        }
        case ViewBackground::NONE:
            break;
    }
}

ActionIdentifier View::registerAction(std::string hintText, enum ControllerButton button, ActionListener actionListener, bool hidden, enum Sound sound)
{
    ActionIdentifier nextIdentifier = (this->actions.size() == 0) ? 1 : this->actions.back().identifier + 1;

    if (auto it = std::find(this->actions.begin(), this->actions.end(), button); it != this->actions.end())
        *it = { button, nextIdentifier, hintText, true, hidden, sound, actionListener };
    else
        this->actions.push_back({ button, nextIdentifier, hintText, true, hidden, sound, actionListener });

    return nextIdentifier;
}

void View::unregisterAction(ActionIdentifier identifier)
{
    auto is_matched_action = [identifier](Action action) {
        return action.identifier == identifier;
    };
    if (auto it = std::find_if(this->actions.begin(), this->actions.end(), is_matched_action); it != this->actions.end())
        this->actions.erase(it);
}

void View::registerClickAction(ActionListener actionListener)
{
    this->registerAction("brls/hints/ok"_i18n, BUTTON_A, actionListener, false, SOUND_CLICK);
}

void View::updateActionHint(enum ControllerButton button, std::string hintText)
{
    if (auto it = std::find(this->actions.begin(), this->actions.end(), button); it != this->actions.end())
        it->hintText = hintText;

    Application::getGlobalHintsUpdateEvent()->fire();
}

void View::setActionAvailable(enum ControllerButton button, bool available)
{
    if (auto it = std::find(this->actions.begin(), this->actions.end(), button); it != this->actions.end())
        it->available = available;
}

void View::setParent(Box* parent, void* parentUserdata)
{
    this->parent         = parent;
    this->parentUserdata = parentUserdata;
}

void* View::getParentUserData()
{
    return this->parentUserdata;
}

bool View::isFocused()
{
    return this->focused;
}

enum Sound View::getFocusSound()
{
    return this->focusSound;
}

Box* View::getParent()
{
    return this->parent;
}

bool View::hasParent()
{
    return this->parent;
}

void View::setDimensions(float width, float height)
{
    YGNodeStyleSetMinWidthPercent(this->ygNode, 0);
    YGNodeStyleSetMinHeightPercent(this->ygNode, 0);

    if (width == View::AUTO)
    {
        YGNodeStyleSetWidthAuto(this->ygNode);
        YGNodeStyleSetMinWidth(this->ygNode, YGUndefined);
    }
    else
    {
        YGNodeStyleSetWidth(this->ygNode, width);
        YGNodeStyleSetMinWidth(this->ygNode, width);
    }

    if (height == View::AUTO)
    {
        YGNodeStyleSetHeightAuto(this->ygNode);
        YGNodeStyleSetMinHeight(this->ygNode, YGUndefined);
    }
    else
    {
        YGNodeStyleSetHeight(this->ygNode, height);
        YGNodeStyleSetMinHeight(this->ygNode, height);
    }

    this->invalidate();
}

void View::setWidth(float width)
{
    YGNodeStyleSetMinWidthPercent(this->ygNode, 0);

    if (width == View::AUTO)
    {
        YGNodeStyleSetWidthAuto(this->ygNode);
        YGNodeStyleSetMinWidth(this->ygNode, YGUndefined);
    }
    else
    {
        YGNodeStyleSetWidth(this->ygNode, width);
        YGNodeStyleSetMinWidth(this->ygNode, width);
    }

    this->invalidate();
}

void View::setHeight(float height)
{
    YGNodeStyleSetMinHeightPercent(this->ygNode, 0);

    if (height == View::AUTO)
    {
        YGNodeStyleSetHeightAuto(this->ygNode);
        YGNodeStyleSetMinHeight(this->ygNode, YGUndefined);
    }
    else
    {
        YGNodeStyleSetHeight(this->ygNode, height);
        YGNodeStyleSetMinHeight(this->ygNode, height);
    }

    this->invalidate();
}

void View::setWidthPercentage(float percentage)
{
    YGNodeStyleSetWidthPercent(this->ygNode, percentage);
    YGNodeStyleSetMinWidthPercent(this->ygNode, percentage);
    this->invalidate();
}

void View::setHeightPercentage(float percentage)
{
    YGNodeStyleSetHeightPercent(this->ygNode, percentage);
    YGNodeStyleSetMinHeightPercent(this->ygNode, percentage);
    this->invalidate();
}

void View::setMaxWidth(float maxWidth)
{
    if (maxWidth == View::AUTO)
        YGNodeStyleSetMaxWidth(this->ygNode, YGUndefined);
    else
        YGNodeStyleSetMaxWidth(this->ygNode, maxWidth);

    this->invalidate();
}

void View::setMaxWidthPercentage(float percentage)
{
    YGNodeStyleSetMaxWidthPercent(this->ygNode, percentage);
    this->invalidate();
}

void View::setMaxHeight(float maxHeight)
{
    if (maxHeight == View::AUTO)
        YGNodeStyleSetMaxHeight(this->ygNode, YGUndefined);
    else
        YGNodeStyleSetMaxHeight(this->ygNode, maxHeight);

    this->invalidate();
}

void View::setMaxHeightPercentage(float percentage)
{
    YGNodeStyleSetMaxHeightPercent(this->ygNode, percentage);
    this->invalidate();
}

void View::setMarginTop(float top)
{
    if (top == brls::View::AUTO)
        YGNodeStyleSetMarginAuto(this->ygNode, YGEdgeTop);
    else
        YGNodeStyleSetMargin(this->ygNode, YGEdgeTop, top);

    this->invalidate();
}

void View::setMarginRight(float right)
{
    if (right == brls::View::AUTO)
        YGNodeStyleSetMarginAuto(this->ygNode, YGEdgeRight);
    else
        YGNodeStyleSetMargin(this->ygNode, YGEdgeRight, right);

    this->invalidate();
}

float View::getMarginRight()
{
    return ntz(YGNodeStyleGetMargin(this->ygNode, YGEdgeRight).value);
}

float View::getMarginLeft()
{
    return ntz(YGNodeStyleGetMargin(this->ygNode, YGEdgeRight).value);
}

void View::setMarginBottom(float bottom)
{
    if (bottom == brls::View::AUTO)
        YGNodeStyleSetMarginAuto(this->ygNode, YGEdgeBottom);
    else
        YGNodeStyleSetMargin(this->ygNode, YGEdgeBottom, bottom);

    this->invalidate();
}

void View::setMarginLeft(float left)
{
    if (left == brls::View::AUTO)
        YGNodeStyleSetMarginAuto(this->ygNode, YGEdgeLeft);
    else
        YGNodeStyleSetMargin(this->ygNode, YGEdgeLeft, left);

    this->invalidate();
}

void View::setMargins(float top, float right, float bottom, float left)
{
    if (top == brls::View::AUTO)
        YGNodeStyleSetMarginAuto(this->ygNode, YGEdgeTop);
    else
        YGNodeStyleSetMargin(this->ygNode, YGEdgeTop, top);

    if (right == brls::View::AUTO)
        YGNodeStyleSetMarginAuto(this->ygNode, YGEdgeRight);
    else
        YGNodeStyleSetMargin(this->ygNode, YGEdgeRight, right);

    if (bottom == brls::View::AUTO)
        YGNodeStyleSetMarginAuto(this->ygNode, YGEdgeBottom);
    else
        YGNodeStyleSetMargin(this->ygNode, YGEdgeBottom, bottom);

    if (left == brls::View::AUTO)
        YGNodeStyleSetMarginAuto(this->ygNode, YGEdgeLeft);
    else
        YGNodeStyleSetMargin(this->ygNode, YGEdgeLeft, left);

    this->invalidate();
}

void View::setPositionTop(float pos)
{
    if (pos == View::AUTO)
        YGNodeStyleSetPosition(this->ygNode, YGEdgeTop, YGUndefined);
    else
        YGNodeStyleSetPosition(this->ygNode, YGEdgeTop, pos);
    this->invalidate();
}

void View::setPositionRight(float pos)
{
    if (pos == View::AUTO)
        YGNodeStyleSetPosition(this->ygNode, YGEdgeRight, YGUndefined);
    else
        YGNodeStyleSetPosition(this->ygNode, YGEdgeRight, pos);
    this->invalidate();
}

void View::setPositionBottom(float pos)
{
    if (pos == View::AUTO)
        YGNodeStyleSetPosition(this->ygNode, YGEdgeBottom, YGUndefined);
    else
        YGNodeStyleSetPosition(this->ygNode, YGEdgeBottom, pos);
    this->invalidate();
}

void View::setPositionLeft(float pos)
{
    if (pos == View::AUTO)
        YGNodeStyleSetPosition(this->ygNode, YGEdgeLeft, YGUndefined);
    else
        YGNodeStyleSetPosition(this->ygNode, YGEdgeLeft, pos);
    this->invalidate();
}

void View::setPositionTopPercentage(float percentage)
{
    YGNodeStyleSetPositionPercent(this->ygNode, YGEdgeTop, percentage);
    this->invalidate();
}

void View::setPositionRightPercentage(float percentage)
{
    YGNodeStyleSetPositionPercent(this->ygNode, YGEdgeRight, percentage);
    this->invalidate();
}

void View::setPositionBottomPercentage(float percentage)
{
    YGNodeStyleSetPositionPercent(this->ygNode, YGEdgeBottom, percentage);
    this->invalidate();
}

void View::setPositionLeftPercentage(float percentage)
{
    YGNodeStyleSetPositionPercent(this->ygNode, YGEdgeLeft, percentage);
    this->invalidate();
}

void View::setPositionType(PositionType type)
{
    switch (type)
    {
        case PositionType::RELATIVE:
            YGNodeStyleSetPositionType(this->ygNode, YGPositionTypeRelative);
            break;
        case PositionType::ABSOLUTE:
            YGNodeStyleSetPositionType(this->ygNode, YGPositionTypeAbsolute);
            break;
    }

    this->invalidate();
}

void View::setGrow(float grow)
{
    YGNodeStyleSetFlexGrow(this->ygNode, grow);
    this->invalidate();
}

void View::setShrink(float shrink)
{
    YGNodeStyleSetFlexShrink(this->ygNode, shrink);
    this->invalidate();
}

void View::setAlignSelf(AlignSelf alignSelf)
{
    switch (alignSelf)
    {
        case AlignSelf::AUTO:
            YGNodeStyleSetAlignSelf(this->ygNode, YGAlignAuto);
            break;
        case AlignSelf::FLEX_START:
            YGNodeStyleSetAlignSelf(this->ygNode, YGAlignFlexStart);
            break;
        case AlignSelf::CENTER:
            YGNodeStyleSetAlignSelf(this->ygNode, YGAlignCenter);
            break;
        case AlignSelf::FLEX_END:
            YGNodeStyleSetAlignSelf(this->ygNode, YGAlignFlexEnd);
            break;
        case AlignSelf::STRETCH:
            YGNodeStyleSetAlignSelf(this->ygNode, YGAlignStretch);
            break;
        case AlignSelf::BASELINE:
            YGNodeStyleSetAlignSelf(this->ygNode, YGAlignBaseline);
            break;
        case AlignSelf::SPACE_BETWEEN:
            YGNodeStyleSetAlignSelf(this->ygNode, YGAlignSpaceBetween);
            break;
        case AlignSelf::SPACE_AROUND:
            YGNodeStyleSetAlignSelf(this->ygNode, YGAlignSpaceAround);
            break;
    }

    this->invalidate();
}

void View::invalidate()
{
    if (YGNodeHasMeasureFunc(this->ygNode))
        YGNodeMarkDirty(this->ygNode);

    if (this->hasParent() && !this->detached)
        this->getParent()->invalidate();
    else
        YGNodeCalculateLayout(this->ygNode, YGUndefined, YGUndefined, YGDirectionLTR);
}

float View::getX()
{
    if (this->detached)
        return this->detachedOriginX + this->translationX;
    else if (this->hasParent())
        return this->getParent()->getX() + YGNodeLayoutGetLeft(this->ygNode) + this->translationX;
    else
        return YGNodeLayoutGetLeft(this->ygNode) + this->translationX;
}

float View::getY()
{
    if (this->detached)
        return this->detachedOriginY + this->translationY;
    else if (this->hasParent())
        return this->getParent()->getY() + YGNodeLayoutGetTop(this->ygNode) + this->translationY;
    else
        return YGNodeLayoutGetTop(this->ygNode) + this->translationY;
}

float View::getHeight(bool includeCollapse)
{
    return YGNodeLayoutGetHeight(this->ygNode) * (includeCollapse ? this->collapseState.getValue() : 1.0f);
}

float View::getWidth()
{
    return YGNodeLayoutGetWidth(this->ygNode);
}

void View::detach()
{
    this->detached = true;
}

void View::setDetachedPosition(float x, float y)
{
    this->detachedOriginX = x;
    this->detachedOriginY = y;
}

bool View::isDetached()
{
    return this->detached;
}

void View::onFocusGained()
{
    this->focused = true;

    Style style = Application::getStyle();

    this->highlightAlpha.reset();
    this->highlightAlpha.addStep(1.0f, style["brls/animations/highlight"], EasingFunction::quadraticOut);
    this->highlightAlpha.start();

    this->focusEvent.fire(this);

    if (this->hasParent())
        this->getParent()->onChildFocusGained(this, this);
}

GenericEvent* View::getFocusEvent()
{
    return &this->focusEvent;
}

void View::onFocusLost()
{
    this->focused = false;

    Style style = Application::getStyle();

    this->highlightAlpha.reset();
    this->highlightAlpha.addStep(0.0f, style["brls/animations/highlight"], EasingFunction::quadraticOut);
    this->highlightAlpha.start();

    if (this->hasParent())
        this->getParent()->onChildFocusLost(this, this);
}

void View::setInFadeAnimation(bool inFadeAnimation)
{
    this->inFadeAnimation = inFadeAnimation;
}

bool View::isTranslucent()
{
    return this->fadeIn || this->inFadeAnimation;
}

void View::show(std::function<void(void)> cb)
{
    this->show(cb, true, this->getShowAnimationDuration(TransitionAnimation::FADE));
}

void View::show(std::function<void(void)> cb, bool animate, float animationDuration)
{
    if (!this->hidden)
    {
        this->onShowAnimationEnd();
        cb();
        return;
    }

    brls::Logger::debug("Showing {}", this->describe());

    this->hidden = false;

    this->fadeIn = true;

    if (animate)
    {
        this->alpha.reset(0.0f);

        this->alpha.addStep(1.0f, animationDuration, EasingFunction::quadraticOut);

        this->alpha.setEndCallback([this, cb](bool finished) {
            this->fadeIn = false;
            this->onShowAnimationEnd();
            cb();
        });

        this->alpha.start();
    }
    else
    {
        this->alpha  = 1.0f;
        this->fadeIn = false;
        this->onShowAnimationEnd();
        cb();
    }
}

void View::hide(std::function<void(void)> cb)
{
    this->hide(cb, true, this->getShowAnimationDuration(TransitionAnimation::FADE));
}

void View::hide(std::function<void(void)> cb, bool animated, float animationDuration)
{
    if (this->hidden)
    {
        cb();
        return;
    }

    brls::Logger::debug("Hiding {}", this->describe());

    this->hidden = true;
    this->fadeIn = false;

    if (animated)
    {
        this->alpha.reset(1.0f);

        this->alpha.addStep(0.0f, animationDuration, EasingFunction::quadraticOut);

        this->alpha.setEndCallback([cb](bool finished) { cb(); });

        this->alpha.start();
    }
    else
    {
        this->alpha = 0.0f;
        cb();
    }
}

float View::getShowAnimationDuration(TransitionAnimation animation)
{
    Style style = Application::getStyle();

    if (animation == TransitionAnimation::SLIDE_LEFT || animation == TransitionAnimation::SLIDE_RIGHT)
        fatal("Slide animation is not supported on views");

    return style["brls/animations/show"];
}

bool View::isHidden()
{
    return this->hidden;
}

void View::overrideTheme(Theme* newTheme)
{
    this->themeOverride = newTheme;
}

void View::onParentFocusGained(View* focusedView)
{
}

void View::onParentFocusLost(View* focusedView)
{
}

void View::setCustomNavigationRoute(FocusDirection direction, View* target)
{
    if (!this->focusable)
        fatal("Only focusable views can have a custom navigation route");

    this->customFocusByPtr[direction] = target;
}

void View::setCustomNavigationRoute(FocusDirection direction, std::string targetId)
{
    if (!this->focusable)
        fatal("Only focusable views can have a custom navigation route");

    this->customFocusById[direction] = targetId;
}

bool View::hasCustomNavigationRouteByPtr(FocusDirection direction)
{
    return this->customFocusByPtr.count(direction) > 0;
}

bool View::hasCustomNavigationRouteById(FocusDirection direction)
{
    return this->customFocusById.count(direction) > 0;
}

View* View::getCustomNavigationRoutePtr(FocusDirection direction)
{
    return this->customFocusByPtr[direction];
}

std::string View::getCustomNavigationRouteId(FocusDirection direction)
{
    return this->customFocusById[direction];
}

View::~View()
{
    this->resetClickAnimation();

    // Parent userdata
    if (this->parentUserdata)
    {
        free(this->parentUserdata);
        this->parentUserdata = nullptr;
    }

    // Focus sanity check
    if (Application::getCurrentFocus() == this)
        Application::giveFocus(nullptr);

    for (tinyxml2::XMLDocument* document : this->boundDocuments)
        delete document;
}

std::string View::getStringXMLAttributeValue(std::string value)
{
    if (startsWith(value, "@i18n/"))
    {
        std::string stringName = value.substr(6);
        return getStr(stringName);
    }

    return value;
}

std::string View::getFilePathXMLAttributeValue(std::string value)
{
    if (startsWith(value, "@res/"))
    {
        std::string resPath = value.substr(5);
        return std::string(BRLS_RESOURCES) + resPath;
    }

    return value;
}

bool View::applyXMLAttribute(std::string name, std::string value)
{
    // String -> string
    if (this->stringAttributes.count(name) > 0)
    {
        if (startsWith(value, "@i18n/"))
        {
            this->stringAttributes[name](View::getStringXMLAttributeValue(value));
            return true;
        }

        this->stringAttributes[name](value);
        return true;
    }

    // File path -> file path
    if (startsWith(value, "@res/"))
    {
        std::string path = View::getFilePathXMLAttributeValue(value);

        if (this->filePathAttributes.count(name) > 0)
        {
            this->filePathAttributes[name](path);
            return true;
        }
        else
        {
            return false; // unknown res
        }
    }
    else
    {
        if (this->filePathAttributes.count(name) > 0)
        {
            this->filePathAttributes[name](value);
            return true;
        }

        // don't return false as it can be anything else
    }

    // Auto -> auto
    if (value == "auto")
    {
        if (this->autoAttributes.count(name) > 0)
        {
            this->autoAttributes[name]();
            return true;
        }
        else
        {
            return false;
        }
    }

    // Ends with px -> float
    if (endsWith(value, "px"))
    {
        // Strip the px and parse the float value
        std::string newFloat = value.substr(0, value.length() - 2);
        try
        {
            float floatValue = std::stof(newFloat);
            if (this->floatAttributes.count(name) > 0)
            {
                this->floatAttributes[name](floatValue);
                return true;
            }
            else
            {
                return false;
            }
        }
        catch (const std::invalid_argument& exception)
        {
            return false;
        }
    }

    // Ends with % -> percentage
    if (endsWith(value, "%"))
    {
        // Strip the % and parse the float value
        std::string newFloat = value.substr(0, value.length() - 1);
        try
        {
            float floatValue = std::stof(newFloat);

            if (floatValue < -100 || floatValue > 100)
                return false;

            if (this->percentageAttributes.count(name) > 0)
            {
                this->percentageAttributes[name](floatValue);
                return true;
            }
            else
            {
                return false;
            }
        }
        catch (const std::invalid_argument& exception)
        {
            return false;
        }
    }
    // Starts with @style -> float
    else if (startsWith(value, "@style/"))
    {
        // Parse the style name
        std::string styleName = value.substr(7); // length of "@style/"
        float value           = Application::getStyle()[styleName]; // will throw logic_error if the metric doesn't exist

        if (this->floatAttributes.count(name) > 0)
        {
            this->floatAttributes[name](value);
            return true;
        }
        else
        {
            return false;
        }
    }
    // Starts with with # -> color
    else if (startsWith(value, "#"))
    {
        // Parse the color
        // #RRGGBB format
        if (value.size() == 7)
        {
            unsigned char r, g, b;
            int result = sscanf(value.c_str(), "#%02hhx%02hhx%02hhx", &r, &g, &b);

            if (result != 3)
                return false;
            else if (this->colorAttributes.count(name) > 0)
            {
                this->colorAttributes[name](nvgRGB(r, g, b));
                return true;
            }
            else
            {
                return false;
            }
        }
        // #RRGGBBAA format
        else if (value.size() == 9)
        {
            unsigned char r, g, b, a;
            int result = sscanf(value.c_str(), "#%02hhx%02hhx%02hhx%02hhx", &r, &g, &b, &a);

            if (result != 4)
                return false;
            else if (this->colorAttributes.count(name) > 0)
            {
                this->colorAttributes[name](nvgRGBA(r, g, b, a));
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    // Starts with @theme -> color
    else if (startsWith(value, "@theme/"))
    {
        // Parse the color name
        std::string colorName = value.substr(7); // length of "@theme/"
        NVGcolor value        = Application::getTheme()[colorName]; // will throw logic_error if the color doesn't exist

        if (this->colorAttributes.count(name) > 0)
        {
            this->colorAttributes[name](value);
            return true;
        }
        else
        {
            return false;
        }
    }
    // Equals true or false -> bool
    else if (value == "true" || value == "false")
    {
        bool boolValue = value == "true" ? true : false;

        if (this->boolAttributes.count(name) > 0)
        {
            this->boolAttributes[name](boolValue);
            return true;
        }
        else
        {
            return false;
        }
    }

    // Valid float -> float, otherwise unknown attribute
    try
    {
        float newValue = std::stof(value);
        if (this->floatAttributes.count(name) > 0)
        {
            this->floatAttributes[name](newValue);
            return true;
        }
        else
        {
            return false;
        }
    }
    catch (const std::invalid_argument& exception)
    {
        return false;
    }
}

void View::applyXMLAttributes(tinyxml2::XMLElement* element)
{
    if (!element)
        return;

    for (const tinyxml2::XMLAttribute* attribute = element->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
    {
        std::string name  = attribute->Name();
        std::string value = std::string(attribute->Value());

        if (!this->applyXMLAttribute(name, value))
            this->printXMLAttributeErrorMessage(element, name, value);
    }
}

bool View::isXMLAttributeValid(std::string attributeName)
{
    return this->knownAttributes.count(attributeName) > 0;
}

View* View::createFromXMLResource(std::string name)
{
    return View::createFromXMLFile(std::string(BRLS_RESOURCES) + "xml/" + name);
}

View* View::createFromXMLString(std::string xml)
{
    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    tinyxml2::XMLError error        = document->Parse(xml.c_str());

    if (error != tinyxml2::XMLError::XML_SUCCESS)
        fatal("Invalid XML when creating View from XML: error " + std::to_string(error));

    tinyxml2::XMLElement* root = document->RootElement();

    if (!root)
        fatal("Invalid XML: no element found");

    View* view = View::createFromXMLElement(root);
    view->bindXMLDocument(document);
    return view;
}

View* View::createFromXMLFile(std::string path)
{
    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    tinyxml2::XMLError error        = document->LoadFile(path.c_str());

    if (error != tinyxml2::XMLError::XML_SUCCESS)
        fatal("Unable to load XML file \"" + path + "\": error " + std::to_string(error));

    tinyxml2::XMLElement* element = document->RootElement();

    if (!element)
        fatal("Unable to load XML file \"" + path + "\": no root element found, is the file empty?");

    View* view = View::createFromXMLElement(element);
    view->bindXMLDocument(document);
    return view;
}

View* View::createFromXMLElement(tinyxml2::XMLElement* element)
{
    if (!element)
        return nullptr;

    std::string viewName = element->Name();

    // Instantiate the view
    View* view = nullptr;

    // Special case where element name is brls:View: create from given XML file.
    // XML attributes are explicitely not passed down to the created view.
    // To create a custom view from XML that you can reuse in other XML files,
    // make a class inheriting brls::Box and use the inflateFromXML* methods.
    if (viewName == "brls:View")
    {
        const tinyxml2::XMLAttribute* xmlAttribute = element->FindAttribute("xml");

        if (xmlAttribute)
            view = View::createFromXMLFile(View::getFilePathXMLAttributeValue(xmlAttribute->Value()));
        else
            fatal("brls:View XML tag must have an \"xml\" attribute");
    }
    // Otherwise look in the register
    else
    {
        if (!Application::XMLViewsRegisterContains(viewName))
            fatal("Unknown XML tag \"" + viewName + "\"");

        view = Application::getXMLViewCreator(viewName)();

        view->applyXMLAttributes(element);
    }

    unsigned count = 0;
    unsigned max   = view->getMaximumAllowedXMLElements();
    for (tinyxml2::XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
    {
        if (count >= max)
            fatal("View \"" + view->describe() + "\" is only allowed to have " + std::to_string(max) + " children XML elements");
        else
            view->handleXMLElement(child);

        count++;
    }

    return view;
}

void View::handleXMLElement(tinyxml2::XMLElement* element)
{
    fatal("Raw views cannot have child XML tags");
}

void View::setMaximumAllowedXMLElements(unsigned max)
{
    this->maximumAllowedXMLElements = max;
}

unsigned View::getMaximumAllowedXMLElements()
{
    return this->maximumAllowedXMLElements;
}

void View::registerCommonAttributes()
{
    // Width
    this->registerAutoXMLAttribute("width", [this] {
        this->setWidth(View::AUTO);
    });

    this->registerFloatXMLAttribute("width", [this](float value) {
        this->setWidth(value);
    });

    this->registerPercentageXMLAttribute("width", [this](float value) {
        this->setWidthPercentage(value);
    });

    // Height
    this->registerAutoXMLAttribute("height", [this] {
        this->setHeight(View::AUTO);
    });

    this->registerFloatXMLAttribute("height", [this](float value) {
        this->setHeight(value);
    });

    this->registerPercentageXMLAttribute("height", [this](float value) {
        this->setHeightPercentage(value);
    });

    // Max width
    this->registerAutoXMLAttribute("maxWidth", [this] {
        this->setMaxWidth(View::AUTO);
    });

    this->registerFloatXMLAttribute("maxWidth", [this](float value) {
        this->setMaxWidth(value);
    });

    this->registerPercentageXMLAttribute("maxWidth", [this](float percentage) {
        this->setMaxWidthPercentage(percentage);
    });

    // Max height
    this->registerAutoXMLAttribute("maxHeight", [this] {
        this->setMaxHeight(View::AUTO);
    });

    this->registerFloatXMLAttribute("maxHeight", [this](float value) {
        this->setMaxHeight(value);
    });

    this->registerPercentageXMLAttribute("maxHeight", [this](float percentage) {
        this->setMaxHeightPercentage(percentage);
    });

    // Grow and shrink
    this->registerFloatXMLAttribute("grow", [this](float value) {
        this->setGrow(value);
    });

    this->registerFloatXMLAttribute("shrink", [this](float value) {
        this->setShrink(value);
    });

    // Alignment
    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "alignSelf", AlignSelf, this->setAlignSelf,
        {
            { "auto", AlignSelf::AUTO },
            { "flexStart", AlignSelf::FLEX_START },
            { "center", AlignSelf::CENTER },
            { "flexEnd", AlignSelf::FLEX_END },
            { "stretch", AlignSelf::STRETCH },
            { "baseline", AlignSelf::BASELINE },
            { "spaceBetween", AlignSelf::SPACE_BETWEEN },
            { "spaceAround", AlignSelf::SPACE_AROUND },
        });

    // Margins top
    this->registerFloatXMLAttribute("marginTop", [this](float value) {
        this->setMarginTop(value);
    });

    this->registerAutoXMLAttribute("marginTop", [this] {
        this->setMarginTop(View::AUTO);
    });

    // Margin right
    this->registerFloatXMLAttribute("marginRight", [this](float value) {
        this->setMarginRight(value);
    });

    this->registerAutoXMLAttribute("marginRight", [this] {
        this->setMarginRight(View::AUTO);
    });

    // Margin bottom
    this->registerFloatXMLAttribute("marginBottom", [this](float value) {
        this->setMarginBottom(value);
    });

    this->registerAutoXMLAttribute("marginBottom", [this] {
        this->setMarginBottom(View::AUTO);
    });

    // Margin left
    this->registerFloatXMLAttribute("marginLeft", [this](float value) {
        this->setMarginLeft(value);
    });

    this->registerAutoXMLAttribute("marginLeft", [this] {
        this->setMarginLeft(View::AUTO);
    });

    // Line
    this->registerColorXMLAttribute("lineColor", [this](NVGcolor color) {
        this->setLineColor(color);
    });

    this->registerFloatXMLAttribute("lineTop", [this](float value) {
        this->setLineTop(value);
    });

    this->registerFloatXMLAttribute("lineRight", [this](float value) {
        this->setLineRight(value);
    });

    this->registerFloatXMLAttribute("lineBottom", [this](float value) {
        this->setLineBottom(value);
    });

    this->registerFloatXMLAttribute("lineLeft", [this](float value) {
        this->setLineLeft(value);
    });

    // Position
    this->registerFloatXMLAttribute("positionTop", [this](float value) {
        this->setPositionTop(value);
    });

    this->registerFloatXMLAttribute("positionRight", [this](float value) {
        this->setPositionRight(value);
    });

    this->registerFloatXMLAttribute("positionBottom", [this](float value) {
        this->setPositionBottom(value);
    });

    this->registerFloatXMLAttribute("positionLeft", [this](float value) {
        this->setPositionLeft(value);
    });

    this->registerPercentageXMLAttribute("positionTop", [this](float value) {
        this->setPositionTopPercentage(value);
    });

    this->registerPercentageXMLAttribute("positionRight", [this](float value) {
        this->setPositionRightPercentage(value);
    });

    this->registerPercentageXMLAttribute("positionBottom", [this](float value) {
        this->setPositionBottomPercentage(value);
    });

    this->registerPercentageXMLAttribute("positionLeft", [this](float value) {
        this->setPositionLeftPercentage(value);
    });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "positionType", PositionType, this->setPositionType,
        {
            { "relative", PositionType::RELATIVE },
            { "absolute", PositionType::ABSOLUTE },
        });

    // Custom focus routes
    this->registerStringXMLAttribute("focusUp", [this](std::string value) {
        this->setCustomNavigationRoute(FocusDirection::UP, value);
    });

    this->registerStringXMLAttribute("focusRight", [this](std::string value) {
        this->setCustomNavigationRoute(FocusDirection::RIGHT, value);
    });

    this->registerStringXMLAttribute("focusDown", [this](std::string value) {
        this->setCustomNavigationRoute(FocusDirection::DOWN, value);
    });

    this->registerStringXMLAttribute("focusLeft", [this](std::string value) {
        this->setCustomNavigationRoute(FocusDirection::LEFT, value);
    });

    // Shape
    this->registerColorXMLAttribute("backgroundColor", [this](NVGcolor value) {
        this->setBackgroundColor(value);
    });

    this->registerColorXMLAttribute("borderColor", [this](NVGcolor value) {
        this->setBorderColor(value);
    });

    this->registerFloatXMLAttribute("borderThickness", [this](float value) {
        this->setBorderThickness(value);
    });

    this->registerFloatXMLAttribute("cornerRadius", [this](float value) {
        this->setCornerRadius(value);
    });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "shadowType", ShadowType, this->setShadowType,
        {
            {
                "none",
                ShadowType::NONE,
            },
            {
                "generic",
                ShadowType::GENERIC,
            },
            {
                "custom",
                ShadowType::CUSTOM,
            },
        });

    // Misc
    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "visibility", Visibility, this->setVisibility,
        {
            { "visible", Visibility::VISIBLE },
            { "invisible", Visibility::INVISIBLE },
            { "gone", Visibility::GONE },
        });

    this->registerStringXMLAttribute("id", [this](std::string value) {
        this->setId(value);
    });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "background", ViewBackground, this->setBackground,
        {
            { "sidebar", ViewBackground::SIDEBAR },
            { "backdrop", ViewBackground::BACKDROP },
        });

    this->registerBoolXMLAttribute("focusable", [this](bool value) {
        this->setFocusable(value);
    });

    this->registerBoolXMLAttribute("wireframe", [this](bool value) {
        this->setWireframeEnabled(value);
    });

    // Highlight
    this->registerBoolXMLAttribute("hideHighlightBackground", [this](bool value) {
        this->setHideHighlightBackground(value);
    });

    this->registerFloatXMLAttribute("highlightPadding", [this](float value) {
        this->setHighlightPadding(value);
    });

    this->registerFloatXMLAttribute("highlightCornerRadius", [this](float value) {
        this->setHighlightCornerRadius(value);
    });
}

void View::setTranslationY(float translationY)
{
    this->translationY = translationY;
}

void View::setTranslationX(float translationX)
{
    this->translationX = translationX;
}

void View::setVisibility(Visibility visibility)
{
    // Only change YG properties and invalidate if going from or to GONE
    if ((this->visibility == Visibility::GONE && visibility != Visibility::GONE) || (this->visibility != Visibility::GONE && visibility == Visibility::GONE))
    {
        if (visibility == Visibility::GONE)
            YGNodeStyleSetDisplay(this->ygNode, YGDisplayNone);
        else
            YGNodeStyleSetDisplay(this->ygNode, YGDisplayFlex);

        this->invalidate();
    }

    this->visibility = visibility;

    if (visibility == Visibility::VISIBLE)
        this->willAppear();
    else
        this->willDisappear();
}

void View::printXMLAttributeErrorMessage(tinyxml2::XMLElement* element, std::string name, std::string value)
{
    if (this->knownAttributes.find(name) != this->knownAttributes.end())
        fatal("Illegal value \"" + value + "\" for \"" + std::string(element->Name()) + "\" XML attribute \"" + name + "\"");
    else
        fatal("Unknown XML attribute \"" + name + "\" for tag \"" + std::string(element->Name()) + "\" (with value \"" + value + "\")");
}

void View::registerFloatXMLAttribute(std::string name, FloatAttributeHandler handler)
{
    this->floatAttributes[name] = handler;
    this->knownAttributes.insert(name);
}

void View::registerPercentageXMLAttribute(std::string name, FloatAttributeHandler handler)
{
    this->percentageAttributes[name] = handler;
    this->knownAttributes.insert(name);
}

void View::registerAutoXMLAttribute(std::string name, AutoAttributeHandler handler)
{
    this->autoAttributes[name] = handler;
    this->knownAttributes.insert(name);
}

void View::registerStringXMLAttribute(std::string name, StringAttributeHandler handler)
{
    this->stringAttributes[name] = handler;
    this->knownAttributes.insert(name);
}

void View::registerColorXMLAttribute(std::string name, ColorAttributeHandler handler)
{
    this->colorAttributes[name] = handler;
    this->knownAttributes.insert(name);
}

void View::registerBoolXMLAttribute(std::string name, BoolAttributeHandler handler)
{
    this->boolAttributes[name] = handler;
    this->knownAttributes.insert(name);
}

void View::registerFilePathXMLAttribute(std::string name, FilePathAttributeHandler handler)
{
    this->filePathAttributes[name] = handler;
    this->knownAttributes.insert(name);
}

float ntz(float value)
{
    return std::isnan(value) ? 0.0f : value;
}

View* View::getView(std::string id)
{
    if (id == this->id)
        return this;

    return nullptr;
}

View* View::getNearestView(std::string id)
{
    // First try a children of ours
    View* child = this->getView(id);

    if (child)
        return child;

    // Then go up one level and try again
    if (this->hasParent())
        return this->getParent()->getNearestView(id);

    return nullptr;
}

void View::setId(std::string id)
{
    if (id == "")
        fatal("ID cannot be empty");

    this->id = id;
}

bool View::isFocusable()
{
    return this->focusable && this->visibility == Visibility::VISIBLE;
}

View* View::getDefaultFocus()
{
    if (this->isFocusable())
        return this;

    return nullptr;
}

void View::bindXMLDocument(tinyxml2::XMLDocument* document)
{
    this->boundDocuments.push_back(document);
}

void View::setWireframeEnabled(bool wireframe)
{
    this->wireframeEnabled = wireframe;
}

} // namespace brls
