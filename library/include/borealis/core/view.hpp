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

#pragma once

#include <cxxabi.h>
#include <nanovg.h>
#include <stdio.h>
#include <tinyxml2.h>
#include <yoga/YGNode.h>

#include <borealis/core/actions.hpp>
#include <borealis/core/animation.hpp>
#include <borealis/core/event.hpp>
#include <borealis/core/frame_context.hpp>
#include <borealis/core/util.hpp>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

// Registers an "enum" XML attribute, which is just a string attribute with a map string -> enum inside
// When using this macro please use the same (wonky) formatting as what you see in Box.cpp or View.cpp
// otherwise clang-format will screw it up
#define BRLS_REGISTER_ENUM_XML_ATTRIBUTE(name, enumType, method, ...)                    \
    this->registerStringXMLAttribute(name, [this](std::string value) {                   \
        std::unordered_map<std::string, enumType> enumMap = __VA_ARGS__;                 \
        if (enumMap.count(value) > 0)                                                    \
            method(enumMap[value]);                                                      \
        else                                                                             \
            fatal("Illegal value \"" + value + "\" for XML attribute \"" + name + "\""); \
    })

// Shortcut to register an A key action (generic click) on a view given its id, that calls any function or method
// To be used in activities or derivates of Box (internally uses the getView() method)
// The function or method must return a boolean (true if the action was consumed, false otherwise) and take a single brls::View*
// parameter (useful to know what was clicked if you use the same listener for multiple views)
#define BRLS_REGISTER_CLICK_BY_ID(id, method)                         \
    this->getView(id)->registerClickAction([this](brls::View* view) { \
        return method(view);                                          \
    })

namespace brls
{

const NVGcolor TRANSPARENT = nvgRGBA(0, 0, 0, 0);

// Focus direction when navigating
enum class FocusDirection
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// View background
enum class ViewBackground
{
    NONE,
    SIDEBAR,
    BACKDROP,
    SHAPE_COLOR,
};

enum class AlignSelf
{
    AUTO,
    FLEX_START,
    CENTER,
    FLEX_END,
    STRETCH,
    BASELINE,
    SPACE_BETWEEN,
    SPACE_AROUND,
};

// View visibility
enum class Visibility
{
    VISIBLE, // the view is visible
    INVISIBLE, // the view is invisible but still takes some space
    GONE, // the view is invisible and doesn't take any space
};

// Position attribute behavior
enum class PositionType
{
    RELATIVE, // position attributes offset the view from the base layout
    ABSOLUTE, // position attributes set the absolute coordinates of the view
};

// The animation to play when
// pushing / popping an activity or
// showing / hiding a view.
enum class TransitionAnimation
{
    FADE, // the old activity fades away and the new one fades in
    SLIDE_LEFT, // the old activity slides out to the left and the new one slides in from the right
    SLIDE_RIGHT, // inverted SLIDE_LEFT
};

// A View shape's shadow type
enum class ShadowType
{
    NONE, // do not draw any shadow around the shape
    GENERIC, // generic all-purpose shadow
    CUSTOM, // customized shadow (use the provided methods to tweak it)
};

extern const NVGcolor transparent;

class View;
class Box;

typedef Event<View*> GenericEvent;
typedef Event<> VoidEvent;

typedef std::function<void(void)> AutoAttributeHandler;
typedef std::function<void(int)> IntAttributeHandler;
typedef std::function<void(float)> FloatAttributeHandler;
typedef std::function<void(std::string)> StringAttributeHandler;
typedef std::function<void(NVGcolor)> ColorAttributeHandler;
typedef std::function<void(bool)> BoolAttributeHandler;
typedef std::function<void(std::string)> FilePathAttributeHandler;

/**
 * Some YG values are NAN if not set, wrecking our
 * calculations if we use them as they are
 */
float ntz(float value);

// Superclass for all the other views
// Lifecycle of a view is :
//   new -> [willAppear -> willDisappear] -> delete
//
// Users have do to the new, the rest of the lifecycle is taken
// care of by the library
//
// willAppear and willDisappear can be called zero or multiple times
// before deletion (in case of a TabLayout for instance)
class View
{
  private:
    ViewBackground background = ViewBackground::NONE;

    void drawBackground(NVGcontext* vg, FrameContext* ctx, Style style);
    void drawShadow(NVGcontext* vg, FrameContext* ctx, Style style, float x, float y, float width, float height);
    void drawBorder(NVGcontext* vg, FrameContext* ctx, Style style, float x, float y, float width, float height);
    void drawHighlight(NVGcontext* vg, Theme theme, float alpha, Style style, bool background);
    void drawClickAnimation(NVGcontext* vg, FrameContext* ctx, float x, float y, float width, float height);
    void drawWireframe(FrameContext* ctx, float x, float y, float width, float height);
    void drawLine(FrameContext* ctx, float x, float y, float width, float height);

    Animatable highlightAlpha   = 0.0f;
    float highlightPadding      = 0.0f;
    float highlightCornerRadius = 0.0f;

    Animatable clickAlpha = 0.0f; // animated between 0 and 1

    bool highlightShaking = false;
    Time highlightShakeStart;
    FocusDirection highlightShakeDirection;
    float highlightShakeAmplitude;

    bool fadeIn          = false; // is the fade in animation running?
    bool inFadeAnimation = false; // is any fade animation running?

    Theme* themeOverride = nullptr;

    bool hidden    = false;
    bool focusable = false;

    enum Sound focusSound = SOUND_FOCUS_CHANGE;

    bool hideHighlightBackground = false;

    bool detached         = false;
    float detachedOriginX = 0.0f;
    float detachedOriginY = 0.0f;

    float translationX = 0.0f;
    float translationY = 0.0f;

    bool wireframeEnabled = false;

    std::vector<Action> actions;

    /**
     * Parent user data, typically the index of the view
     * in the internal layout structure
     */
    void* parentUserdata = nullptr;

    bool culled = true; // will be culled by the parent Box, if any

    std::vector<tinyxml2::XMLDocument*> boundDocuments;

    std::unordered_map<std::string, AutoAttributeHandler> autoAttributes;
    std::unordered_map<std::string, FloatAttributeHandler> percentageAttributes;
    std::unordered_map<std::string, FloatAttributeHandler> floatAttributes;
    std::unordered_map<std::string, StringAttributeHandler> stringAttributes;
    std::unordered_map<std::string, ColorAttributeHandler> colorAttributes;
    std::unordered_map<std::string, BoolAttributeHandler> boolAttributes;
    std::unordered_map<std::string, FilePathAttributeHandler> filePathAttributes;

    std::set<std::string> knownAttributes;

    void registerCommonAttributes();
    void printXMLAttributeErrorMessage(tinyxml2::XMLElement* element, std::string name, std::string value);

    unsigned maximumAllowedXMLElements = UINT_MAX;

    NVGcolor lineColor = TRANSPARENT;
    float lineTop      = 0;
    float lineRight    = 0;
    float lineBottom   = 0;
    float lineLeft     = 0;

    Visibility visibility = Visibility::VISIBLE;

    NVGcolor backgroundColor = TRANSPARENT;

    NVGcolor borderColor  = TRANSPARENT;
    float borderThickness = 0.0f;
    float cornerRadius    = 0.0f;
    ShadowType shadowType = ShadowType::NONE;
    bool showShadow       = true;

    std::unordered_map<FocusDirection, std::string> customFocusById;
    std::unordered_map<FocusDirection, View*> customFocusByPtr;

  protected:
    Animatable collapseState = 1.0f;

    bool focused = false;

    Box* parent = nullptr;

    GenericEvent focusEvent;

    YGNode* ygNode;

    std::string id = "";

    // Helper functions to apply this view's alpha to a color
    NVGcolor a(NVGcolor color);
    NVGpaint a(NVGpaint paint);

    NVGcolor RGB(unsigned r, unsigned g, unsigned b)
    {
        return this->a(nvgRGB(r, g, b));
    }

    NVGcolor RGBA(unsigned r, unsigned g, unsigned b, unsigned a)
    {
        return this->a(nvgRGBA(r, g, b, a));
    }

    NVGcolor RGBf(float r, float g, float b)
    {
        return this->a(nvgRGBf(r, g, b));
    }

    NVGcolor RGBAf(float r, float g, float b, float a)
    {
        return this->a(nvgRGBAf(r, g, b, a));
    }

    /**
     * Should the hint alpha be animated when
     * pushing the view?
     */
    virtual bool animateHint()
    {
        return false;
    }

  public:
    static constexpr float AUTO = NAN;

    View();
    virtual ~View();

    void setBackground(ViewBackground background);

    void shakeHighlight(FocusDirection direction);

    float getX();
    float getY();
    float getWidth();
    float getHeight(bool includeCollapse = true);

    /**
    * Triggers a layout of the whole view tree. Must be called
    * after a yoga node property is changed.
    *
    * Only methods that change yoga nodes properties should
    * call this method.
    */
    void invalidate();

    /**
     * Called when a layout pass ends on that view.
     */
    virtual void onLayout() {};

    /**
     * Returns the view with the corresponding id in the view or its children,
     * or nullptr if it hasn't been found.
     *
     * Research is done recursively by traversing the tree starting from this view.
     * This view's parents are not traversed.
     */
    virtual View* getView(std::string id);

    // -----------------------------------------------------------
    // Flex layout properties
    // -----------------------------------------------------------

    /**
     * Sets the preferred width of the view. Use brls::View::AUTO
     * to have the layout automatically resize the view.
     *
     * If set to anything else than AUTO, the view is guaranteed
     * to never shrink below the given width.
     */
    void setWidth(float width);

    /**
     * Sets the preferred height of the view. Use brls::View::AUTO
     * to have the layout automatically resize the view.
     *
     * If set to anything else than AUTO, the view is guaranteed
     * to never shrink below the given height.
     */
    void setHeight(float height);

    /**
     * Shortcut to setWidth + setHeight.
     *
     * Only does one layout pass instead of two when using the two methods separately.
     */
    void setDimensions(float width, float height);

    /**
     * Sets the preferred width of the view in percentage of
     * the parent view width. Between 0.0f and 100.0f.
     */
    void setWidthPercentage(float percentage);

    /**
     * Sets the preferred height of the view in percentage of
     * the parent view height. Between 0.0f and 100.0f.
     */
    void setHeightPercentage(float percentage);

    /**
     * Sets the maximum width of the view, in pixels.
     *
     * This constraint is stronger than the grow factor: the view
     * is guaranteed to never be larger than the given max width.
     *
     * Use View::AUTO to disable the max width constraint.
     */
    void setMaxWidth(float maxWidth);

    /**
     * Sets the maximum height of the view, in pixels.
     *
     * This constraint is stronger than the grow factor: the view
     * is guaranteed to never be larger than the given max height.
     *
     * Use View::AUTO to disable the max height constraint.
     */
    void setMaxHeight(float maxHeight);

    /**
     * Sets the maximum width of the view, in parent width percentage.
     *
     * This constraint is stronger than the grow factor: the view
     * is guaranteed to never be larger than the given max width.
     *
     * Use View::AUTO to disable the max width constraint.
     */
    void setMaxWidthPercentage(float percentage);

    /**
     * Sets the maximum height of the view, in parent height percentage.
     *
     * This constraint is stronger than the grow factor: the view
     * is guaranteed to never be larger than the given max height.
     *
     * Use View::AUTO to disable the max height constraint.
     */
    void setMaxHeightPercentage(float percentage);

    /**
     * Sets the grow factor of the view, aka the percentage
     * of remaining space to give this view, in the containing box axis.
     * Opposite of shrink.
     * Default is 0.0f;
     */
    void setGrow(float grow);

    /**
     * Sets the shrink factor of the view, aka the percentage of space
     * the view is allowed to shrink for if there is not enough space for everyone
     * in the contaning box axis. Opposite of grow.
     * Default is 1.0f;
     */
    void setShrink(float shrink);

    /**
     * Sets the margin of the view, aka the space that separates
     * this view and the surrounding ones in all 4 directions.
     *
     * Use brls::View::AUTO to have the layout automatically select the
     * margin.
     *
     * Only works with views that have parents - top level views that are pushed
     * on the stack don't have parents.
     *
     * Only does one layout pass instead of four when using the four methods separately.
     */
    void setMargins(float top, float right, float bottom, float left);

    /**
     * Sets the top margin of the view, aka the space that separates
     * this view and the surrounding ones.
     *
     * Only works with views that have parents - top level views that are pushed
     * on the stack don't have parents.
     *
     * Use brls::View::AUTO to have the layout automatically select the
     * margin.
     */
    void setMarginTop(float top);

    /**
     * Sets the right margin of the view, aka the space that separates
     * this view and the surrounding ones.
     *
     * Only works with views that have parents - top level views that are pushed
     * on the stack don't have parents.
     *
     * Use brls::View::AUTO to have the layout automatically select the
     * margin.
     */
    void setMarginRight(float right);

    float getMarginRight();
    float getMarginLeft();

    /**
     * Sets the bottom margin of the view, aka the space that separates
     * this view and the surrounding ones.
     *
     * Only works with views that have parents - top level views that are pushed
     * on the stack don't have parents.
     *
     * Use brls::View::AUTO to have the layout automatically select the
     * margin.
     */
    void setMarginBottom(float right);

    /**
     * Sets the right margin of the view, aka the space that separates
     * this view and the surrounding ones.
     *
     * Only works with views that have parents - top level views that are pushed
     * on the stack don't have parents.
     *
     * Use brls::View::AUTO to have the layout automatically select the
     * margin.
     */
    void setMarginLeft(float left);

    /**
     * Sets the visibility of the view.
     */
    void setVisibility(Visibility visibility);

    /**
     * Sets the top position of the view, in pixels.
     *
     * The behavior of this attribute changes depending on the
     * position type of the view.
     *
     * If relative, it will simply offset the view by the given amount.
     *
     * If absolute, it will behave like the "display: absolute;" CSS property
     * and move the view freely in its parent. Use 0 to snap to the parent top edge.
     * Absolute positioning ignores padding.
     *
     * Use View::AUTO to disable (not the same as 0).
     */
    void setPositionTop(float pos);

    /**
     * Sets the right position of the view, in pixels.
     *
     * The behavior of this attribute changes depending on the
     * position type of the view.
     *
     * If relative, it will simply offset the view by the given amount.
     *
     * If absolute, it will behave like the "display: absolute;" CSS property
     * and move the view freely in its parent. Use 0 to snap to the parent right edge.
     * Absolute positioning ignores padding.
     *
     * Use View::AUTO to disable (not the same as 0).
     */
    void setPositionRight(float pos);

    /**
     * Sets the bottom position of the view, in pixels.
     *
     * The behavior of this attribute changes depending on the
     * position type of the view.
     *
     * If relative, it will simply offset the view by the given amount.
     *
     * If absolute, it will behave like the "display: absolute;" CSS property
     * and move the view freely in its parent. Use 0 to snap to the parent bottom edge.
     * Absolute positioning ignores padding.
     *
     * Use View::AUTO to disable (not the same as 0).
     */
    void setPositionBottom(float pos);

    /**
     * Sets the left position of the view, in pixels.
     *
     * The behavior of this attribute changes depending on the
     * position type of the view.
     *
     * If relative, it will simply offset the view by the given amount.
     *
     * If absolute, it will behave like the "display: absolute;" CSS property
     * and move the view freely in its parent. Use 0 to snap to the parent left edge.
     * Absolute positioning ignores padding.
     *
     * Use View::AUTO to disable (not the same as 0).
     */
    void setPositionLeft(float pos);

    /**
     * Sets the top position of the view, in percents.
     *
     * The behavior of this attribute changes depending on the
     * position type of the view.
     */
    void setPositionTopPercentage(float percentage);

    /**
     * Sets the right position of the view, in percents.
     *
     * The behavior of this attribute changes depending on the
     * position type of the view.
     */
    void setPositionRightPercentage(float percentage);

    /**
     * Sets the bottom position of the view, in percentage.
     *
     * The behavior of this attribute changes depending on the
     * position type of the view.
     */
    void setPositionBottomPercentage(float percentage);

    /**
     * Sets the left position of the view, in percents.
     *
     * The behavior of this attribute changes depending on the
     * position type of the view.
     */
    void setPositionLeftPercentage(float percentage);

    /**
     * Sets the "position type" of the view, aka the behavior
     * of the 4 position attributes.
     *
     * Default is RELATIVE.
     */
    void setPositionType(PositionType type);

    /**
     * Sets the id of the view.
     */
    void setId(std::string id);

    /**
     * Overrides align items of the parent box.
     *
     * Default is AUTO.
     */
    void setAlignSelf(AlignSelf align);

    // -----------------------------------------------------------
    // Styling and view shape properties
    // -----------------------------------------------------------

    /**
     * Sets the line color for the view. To be used with setLineTop(),
     * setLineRight()...
     *
     * The "line" is separate from the shape "border".
     */
    inline void setLineColor(NVGcolor color)
    {
        this->lineColor = color;
    }

    /**
     * Sets the top line thickness. Use setLineColor()
     * to change the line color.
     *
     * The "line" is separate from the shape "border".
     */
    inline void setLineTop(float thickness)
    {
        this->lineTop = thickness;
    }

    /**
     * Sets the right line thickness. Use setLineColor()
     * to change the line color.
     *
     * The "line" is separate from the shape "border".
     */
    inline void setLineRight(float thickness)
    {
        this->lineRight = thickness;
    }

    /**
     * Sets the bottom line thickness. Use setLineColor()
     * to change the line color.
     *
     * The "line" is separate from the shape "border".
     */
    inline void setLineBottom(float thickness)
    {
        this->lineBottom = thickness;
    }

    /**
     * Sets the left line thickness. Use setLineColor()
     * to change the line color.
     *
     * The "line" is separate from the shape "border".
     */
    inline void setLineLeft(float thickness)
    {
        this->lineLeft = thickness;
    }

    /**
     * Sets the view shape background color.
     */
    inline void setBackgroundColor(NVGcolor color)
    {
        this->backgroundColor = color;
        this->setBackground(ViewBackground::SHAPE_COLOR);
    }

    /**
     * Sets the view shape border color.
     */
    inline void setBorderColor(NVGcolor color)
    {
        this->borderColor = color;
    }

    /**
     * Sets the view shape border thickness.
     */
    inline void setBorderThickness(float thickness)
    {
        this->borderThickness = thickness;
    }

    inline float getBorderThickness()
    {
        return this->borderThickness;
    }

    /**
     * Sets the view shape corner radius.
     * 0 means no rounded corners.
     */
    inline void setCornerRadius(float radius)
    {
        this->cornerRadius = radius;
    }

    /**
     * Sets the view shape shadow type.
     * Default is NONE.
     */
    inline void setShadowType(ShadowType type)
    {
        this->shadowType = type;
    }

    /**
     * Sets the shadow visibility.
     */
    inline void setShadowVisibility(bool visible)
    {
        this->showShadow = visible;
    }

    /**
     * If set to true, the highlight background will be hidden for this view
     * (the white rectangle that goes behind the view, replacing the usual background shape).
     */
    inline void setHideHighlightBackground(bool hide)
    {
        this->hideHighlightBackground = hide;
    }

    /**
     * Sets the highlight padding of the view, aka the space between the
     * highlight rectangle and the view. The highlight rect is enlarged, the view is untouched.
     */
    inline void setHighlightPadding(float padding)
    {
        this->highlightPadding = padding;
    }

    /**
     * Sets the highlight rectangle corner radius.
     */
    inline void setHighlightCornerRadius(float radius)
    {
        this->highlightCornerRadius = radius;
    }

    // -----------------------------------------------------------

    /**
     * Returns the "nearest" view with the corresponding id, or nullptr if none has
     * been found. "Nearest" means the closest in the vicinity
     * of this view. The siblings are searched as well as its children.
     *
     * Research is done by traversing the tree upwards, starting from this view.
     * The current algorithm is very inefficient.
     */
    virtual View* getNearestView(std::string id);

    /**
     * Creates a view from the given XML file content.
     *
     * The method handleXMLElement() is executed for each child node in the XML.
     *
     * Uses the internal lookup table to instantiate the views.
     * Use registerXMLView() to add your own views to the table so that
     * you can use them in your own XML files.
     */
    static View* createFromXMLString(std::string xml);

    /**
     * Creates a view from the given XML element (node and attributes).
     *
     * The method handleXMLElement() is executed for each child node in the XML.
     *
     * Uses the internal lookup table to instantiate the views.
     * Use registerXMLView() to add your own views to the table so that
     * you can use them in your own XML files.
     */
    static View* createFromXMLElement(tinyxml2::XMLElement* element);

    /**
     * Creates a view from the given XML file path.
     *
     * The method handleXMLElement() is executed for each child node in the XML.
     *
     * Uses the internal lookup table to instantiate the views.
     * Use registerXMLView() to add your own views to the table so that
     * you can use them in your own XML files.
     */
    static View* createFromXMLFile(std::string path);

    /**
     * Creates a view from the given XML resource file name.
     *
     * The method handleXMLElement() is executed for each child node in the XML.
     *
     * Uses the internal lookup table to instantiate the views.
     * Use registerXMLView() to add your own views to the table so that
     * you can use them in your own XML files.
     */
    static View* createFromXMLResource(std::string name);

    /**
     * Handles a child XML element.
     *
     * You can redefine this method to handle child XML like
     * as you want in your own views.
     *
     * If left unimplemented, will throw an exception because raw
     * views cannot handle child XML elements (Boxes can).
     */
    virtual void handleXMLElement(tinyxml2::XMLElement* element);

    /**
     * Applies the attributes of the given XML element to the view.
     *
     * You can add your own attributes to by calling registerXMLAttribute()
     * in the view constructor.
     */
    virtual void applyXMLAttributes(tinyxml2::XMLElement* element);

    /**
     * Applies the given attribute to the view.
     *
     * You can add your own attributes to by calling registerXMLAttribute()
     * in the view constructor.
     */
    virtual bool applyXMLAttribute(std::string name, std::string value);

    /**
     * Register a new XML attribute with the given name and handler
     * method. You can have multiple attributes registered with the same
     * name but different types / handlers, except if the type is string.
     *
     * The method will be called if the attribute has the value "auto".
     */
    void registerAutoXMLAttribute(std::string name, AutoAttributeHandler handler);

    /**
     * Register a new XML attribute with the given name and handler
     * method. You can have multiple attributes registered with the same
     * name but different types / handlers, except if the type is string.
     *
     * The method will be called if the attribute has a percentage value (an integer with "%" suffix).
     * The given float value is guaranteed to be between 0.0f and 1.0f.
     */
    void registerPercentageXMLAttribute(std::string name, FloatAttributeHandler handler);

    /**
     * Register a new XML attribute with the given name and handler
     * method. You can have multiple attributes registered with the same
     * name but different types / handlers, except if the type is string.
     *
     * The method will be called if the attribute has an integer, float, @style or "px" value.
     */
    void registerFloatXMLAttribute(std::string name, FloatAttributeHandler handler);

    /**
     * Register a new XML attribute with the given name and handler
     * method. You can have multiple attributes registered with the same
     * name but different types / handlers, except if the type is string.
     *
     * The method will be called if the attribute has a string or @i18n value.
     *
     * If you use string as a type, you can only have one handler for the attribute.
     */
    void registerStringXMLAttribute(std::string name, StringAttributeHandler handler);

    /**
     * Register a new XML attribute with the given name and handler
     * method. You can have multiple attributes registered with the same
     * name but different types / handlers, except if the type is string.
     *
     * The method will be called if the attribute has a color value ("#XXXXXX" or "#XXXXXXXX")
     * or a @theme value.
     */
    void registerColorXMLAttribute(std::string name, ColorAttributeHandler handler);

    /**
     * Register a new XML attribute with the given name and handler
     * method. You can have multiple attributes registered with the same
     * name but different types / handlers, except if the type is string.
     *
     * The method will be called if the attribute has a boolean value ("true" or "false").
     */
    void registerBoolXMLAttribute(std::string name, BoolAttributeHandler handler);

    /**
     * Register a new XML attribute with the given name and handler
     * method. You can have multiple attributes registered with the same
     * name but different types / handlers, except if the type is string.
     *
     * The method will be called if the attribute has a file path value ("@res/" or raw path).
     */
    void registerFilePathXMLAttribute(std::string name, FilePathAttributeHandler handler);

    /**
     * Binds the given XML document to the view for ownership. The
     * document will then be deleted when the view is.
     */
    void bindXMLDocument(tinyxml2::XMLDocument* document);

    /**
     * Returns if the given XML attribute name is valid for that view.
     */
    bool isXMLAttributeValid(std::string attributeName);

    /**
     * Sets the maximum number of allowed children XML elements
     * when using a view of that class in an XML file.
     */
    void setMaximumAllowedXMLElements(unsigned max);

    unsigned getMaximumAllowedXMLElements();

    /**
     * If set to true, will force the view to be translucent.
     */
    void setInFadeAnimation(bool translucent);

    /**
     * Sets the view to be focusable.
     *
     * Required to be able to use actions that need
     * focus on that view (such as an A press).
     */
    inline void setFocusable(bool focusable)
    {
        this->focusable = focusable;
    }

    bool isFocusable();

    /**
     * Sets the sound to play when this view gets focused.
     */
    inline void setFocusSound(enum Sound sound)
    {
        this->focusSound = sound;
    }

    enum Sound getFocusSound();

    /**
     * Sets the detached flag to true.
     * This action is irreversible.
     *
     * A detached view will, as the name suggests, not be
     * attached to their parent Yoga node. That means that invalidation
     * and layout need to be taken care of manually by the parent.
     *
     * detach() must be called before adding the view to the parent.
     */
    void detach();

    bool isDetached();

    /**
     * Sets the position of the view, if detached.
     */
    void setDetachedPosition(float x, float y);

    void setParent(Box* parent, void* parentUserdata = nullptr);
    Box* getParent();
    bool hasParent();

    void* getParentUserData();

    /**
     * Registers an action with the given parameters. The listener will be fired when the user presses
     * the key when the view is focused.
     *
     * The listener should return true if the action was consumed, false otherwise.
     * The sound will only be played if the listener returned true.
     *
     * A hidden action will not show up in the bottom-right hints.
     *
     * Returns the identifier for the action, so it can be unregistered later on. Returns ACTION_NONE if the
     * action was not registered.
     */
    ActionIdentifier registerAction(std::string hintText, enum ControllerButton button, ActionListener actionListener, bool hidden = false, enum Sound sound = SOUND_NONE);

    /**
     * Unregisters an action with the given identifier.
     */
    void unregisterAction(ActionIdentifier identifier);

    /**
     * Shortcut to register a generic "A OK" click action.
     */
    void registerClickAction(ActionListener actionListener);

    void updateActionHint(enum ControllerButton button, std::string hintText);
    void setActionAvailable(enum ControllerButton button, bool available);

    void resetClickAnimation();
    void playClickAnimation(bool reverse = false);

    std::string getClassString() const
    {
        // Taken from: https://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname/4541470#4541470
        const char* name = typeid(*this).name();
        int status       = 0;

        std::unique_ptr<char, void (*)(void*)> res {
            abi::__cxa_demangle(name, NULL, NULL, &status),
            std::free
        };

        return (status == 0) ? res.get() : name;
    }

    std::string describe() const
    {
        std::string classString = this->getClassString();

        if (this->id != "")
            return classString + " (id=\"" + this->id + "\")";

        return classString;
    }

    YGNode* getYGNode()
    {
        return this->ygNode;
    }

    const std::vector<Action>& getActions()
    {
        return this->actions;
    }

    /**
      * Called each frame
      * Do not override it to draw your view,
      * override draw() instead
      */
    virtual void frame(FrameContext* ctx);

    /**
      * Called by frame() to draw the view onscreen.
      * Views should not draw outside of their bounds (they
      * may be clipped if they do so).
      */
    virtual void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) = 0;

    /**
      * Called when the view will appear
      * on screen, before or after layout().
      *
      * Can be called if the view has
      * already appeared, so be careful.
      */
    virtual void willAppear(bool resetState = false)
    {
        // Nothing to do
    }

    /**
      * Called when the view will disappear
      * from the screen.
      *
      * Can be called if the view has
      * already disappeared, so be careful.
      */
    virtual void willDisappear(bool resetState = false)
    {
        // Nothing to do
    }

    /**
      * Called when the show() animation (fade in)
      * ends
      */
    virtual void onShowAnimationEnd() {};

    /**
      * Shows the view with a fade in animation.
      */
    virtual void show(std::function<void(void)> cb);

    /**
      * Shows the view with a fade in animation, or no animation at all.
      */
    virtual void show(std::function<void(void)> cb, bool animate, float animationDuration);

    /**
     * Returns the duration of the view show / hide animation.
     */
    virtual float getShowAnimationDuration(TransitionAnimation animation);

    /**
      * Hides the view in a collapse animation
      */
    void collapse(bool animated = true);

    bool isCollapsed();

    void setAlpha(float alpha);

    /**
      * Shows the view in a expand animation (opposite
      * of collapse)
      */
    void expand(bool animated = true);

    /**
      * Hides the view with a fade out animation.
      */
    virtual void hide(std::function<void(void)> cb);

    /**
      * Hides the view with a fade out animation, or no animation at all.
      */
    virtual void hide(std::function<void(void)> cb, bool animate, float animationDuration);

    bool isHidden();

    /**
      * Is this view translucent?
      *
      * If you override it please return
      * <value> || View::isTranslucent()
      * to keep the fadeIn transition
      */
    virtual bool isTranslucent();

    bool isFocused();

    /**
     * Returns the default view to focus when focusing this view
     * Typically the view itself or one of its children.
     *
     * Returning nullptr means that the view is not focusable
     * (and neither are its children)
     *
     * By default, a view is focusable if the flag is set to true with setFocusable()
     * and if the view is visible.
     *
     * When pressing a key, the flow is :
     *    1. starting from the currently focused view's parent, traverse the tree upwards and
     *       repeatedly call getNextFocus() on every view until we find a next view to focus or meet the end of the tree
     *    2. if a view is found, getNextFocus() will internally call getDefaultFocus() for the selected child
     *    3. give focus to the result, if it exists
     */
    virtual View* getDefaultFocus();

    /**
     * Returns the next view to focus given the requested direction
     * and the currently focused view (as parent user data)
     *
     * Returning nullptr means that there is no next view to focus
     * in that direction - getNextFocus will then be called on our
     * parent if any
     */
    virtual View* getNextFocus(FocusDirection direction, View* currentView)
    {
        return nullptr;
    }

    /**
     * Sets a custom navigation route from this view to the target one.
     */
    void setCustomNavigationRoute(FocusDirection direction, View* target);

    /**
     * Sets a custom navigation route from this view to the target one, by ID.
     * The final target view will be the "nearest" with the given ID.
     *
     * Resolution of the ID to View is made when the navigation event occurs, not when the
     * route is registered.
     */
    void setCustomNavigationRoute(FocusDirection direction, std::string targetId);

    bool hasCustomNavigationRouteByPtr(FocusDirection direction);
    bool hasCustomNavigationRouteById(FocusDirection direction);

    View* getCustomNavigationRoutePtr(FocusDirection direction);
    std::string getCustomNavigationRouteId(FocusDirection direction);

    /**
      * Fired when focus is gained.
      */
    virtual void onFocusGained();

    /**
      * Fired when focus is lost.
      */
    virtual void onFocusLost();

    /**
     * Fired when focus is gained on this view's parent, or the parent of the parent...
     */
    virtual void onParentFocusGained(View* focusedView);

    /**
     * Fired when focus is lost on one of this view's parents. Works similarly to
     * onParentFocusGained().
     */
    virtual void onParentFocusLost(View* focusedView);

    /**
     * Fired when the window size changes, after updating
     * layout.
     */
    virtual void onWindowSizeChanged()
    {
        // Nothing by default
    }

    GenericEvent* getFocusEvent();

    Animatable alpha = 1.0f;

    virtual float getAlpha(bool child = false);

    /**
      * Forces this view and its children to use
      * the specified theme.
      */
    void overrideTheme(Theme* newTheme);

    /**
     * Enables / disable culling for that view.
     *
     * To disable culling for all child views
     * of a Box use setCullingEnabled on the box.
     */
    void setCulled(bool culled)
    {
        this->culled = culled;
    }

    bool isCulled()
    {
        return this->culled;
    }

    /**
     * Sets the Y translation of this view.
     *
     * Translation is applied after the layout phase. Use relative position
     * and setPosition methods if possible instead.
     */
    void setTranslationY(float translateY);

    /**
     * Sets the X translation of this view.
     *
     * Translation is applied after the layout phase. Use relative position
     * and setPosition methods if possible instead.
     */
    void setTranslationX(float translateX);

    /**
     * Wireframe mode allows you to see the view size and margins (and
     * padding if applicable) directly in your app.
     *
     * Useful to diagnose views misplacements or display bugs.
     */
    void setWireframeEnabled(bool wireframe);

    /**
     * Resolves the value of the given XML attribute string.
     * Applies i18n if the value is an "@i18n/" string, returns the
     * string as it is otherwise.
     */
    static std::string getStringXMLAttributeValue(std::string value);

    /**
     * Resolves the value of the given XML attribute file path.
     * Returns the full path of the resource if it starts with "@res/", returns the
     * path as it is otherwise.
     */
    static std::string getFilePathXMLAttributeValue(std::string value);
};

} // namespace brls
