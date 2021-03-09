/*
    Copyright 2020-2021 natinusala

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

#include <borealis/core/view.hpp>

namespace brls
{

enum class JustifyContent
{
    FLEX_START,
    CENTER,
    FLEX_END,
    SPACE_BETWEEN,
    SPACE_AROUND,
    SPACE_EVENLY,
};

enum class AlignItems
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

enum class Axis
{
    ROW,
    COLUMN,
};

enum class Direction
{
    INHERIT,
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT,
};

// Generic FlexBox layout
class Box : public View
{
  public:
    Box(Axis flexDirection);
    Box();

    void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) override;
    View* getDefaultFocus() override;
    View* getNextFocus(FocusDirection direction, View* currentView) override;
    void willAppear(bool resetState) override;
    void willDisappear(bool resetState) override;
    void onWindowSizeChanged() override;
    void onFocusGained() override;
    void onFocusLost() override;
    void onParentFocusGained(View* focusedView) override;
    void onParentFocusLost(View* focusedView) override;
    bool applyXMLAttribute(std::string name, std::string value) override;

    static View* create();

    /**
     * Adds a view to this Box.
     * Returns the position the view was added at.
     */
    virtual void addView(View* view);

    /**
     * Adds a view to this Box at the given position.
     * Returns the position the view was added at.
     */
    virtual void addView(View* view, size_t position);

    /**
     * Removes the given view from the Box. It will be freed.
     */
    virtual void removeView(View* view);

    /**
     * Sets the padding of the view, aka the internal space to give
     * between this view boundaries and its children.
     *
     * Only does one layout pass instead of four when using the four methods separately.
     */
    virtual void setPadding(float padding);

    /**
     * Sets the padding of the view, aka the internal space to give
     * between this view boundaries and its children.
     *
     * Only does one layout pass instead of four when using the four methods separately.
     */
    virtual void setPadding(float top, float right, float bottom, float left);

    /**
     * Sets the padding of the view, aka the internal space to give
     * between this view boundaries and its children.
     */
    virtual void setPaddingTop(float top);

    /**
     * Sets the padding of the view, aka the internal space to give
     * between this view boundaries and its children.
     */
    virtual void setPaddingRight(float right);

    /**
     * Sets the padding of the view, aka the internal space to give
     * between this view boundaries and its children.
     */
    virtual void setPaddingBottom(float bottom);

    /**
     * Sets the padding of the view, aka the internal space to give
     * between this view boundaries and its children.
     */
    virtual void setPaddingLeft(float left);

    /**
     * Sets the children alignment along the Box axis.
     *
     * Default is FLEX_START.
     */
    void setJustifyContent(JustifyContent justify);

    /**
     * Sets the children alignment along the Box cross axis.
     *
     * Default is AUTO.
     */
    void setAlignItems(AlignItems alignment);

    /**
     * Sets the direction of the box, aka place the views
     * left to right or right to left (flips the children).
     *
     * Default is INHERIT.
     */
    void setDirection(Direction direction);

    void setAxis(Axis axis);

    std::vector<View*>& getChildren();

    /**
     * Returns the bounds used for culling children.
     */
    virtual void getCullingBounds(float* top, float* right, float* bottom, float* left);

    /**
     * Registers an XML attribute to be forwarded to the given view. Works regardless of the target attribute type.
     * Useful to expose attributes of children views in the parent box without copy pasting them individually.
     *
     * The forwarded attribute value will override the value of the regular attribute if it already exists in the target view.
     */
    void forwardXMLAttribute(std::string attributeName, View* target);

    /**
     * Registers an XML attribute to be forwarded to the given view, while changing the target attribute name.
     * Works regardless of the target attribute type.
     * Useful to expose attributes of children views in the parent box without copy pasting them individually, but with a different name.
     *
     * The forwarded attribute value will override the value of the regular attribute if it already exists in the target view.
     */
    void forwardXMLAttribute(std::string attributeName, View* target, std::string targetAttributeName);

    /**
     * Fired when focus is gained on one of this view's children, or one of the children
     * of the children...
     *
     * directChild is guaranteed to be one of your children. It may not be the view that has been
     * focused.
     *
     * If focusedView == directChild, then the child of yours has been focused.
     * Otherwise, focusedView is a child of directChild.
     */
    virtual void onChildFocusGained(View* directChild, View* focusedView);

    /**
     * Fired when focus is lost on one of this view's children. Works similarly to
     * onChildFocusGained().
     */
    virtual void onChildFocusLost(View* directChild, View* focusedView);

    View* getView(std::string id) override;

  private:
    Axis axis;

    std::vector<View*> children;

    size_t defaultFocusedIndex = 0;

    std::unordered_map<std::string, std::pair<std::string, View*>> forwardedAttributes;

  protected:
    /**
     * Inflates the Box with the given XML string.
     *
     * The root element MUST be a brls::Box, corresponding to the inflated Box itself. Its
     * attributes will be applied to the Box.
     *
     * Each child node in the root brls::Box will be treated as a view and added
     * as a child of the Box.
     */
    void inflateFromXMLString(std::string xml);

    /**
     * Inflates the Box with the given XML element.
     *
     * The root element MUST be a brls::Box, corresponding to the inflated Box itself. Its
     * attributes will be applied to the Box.
     *
     * Each child node in the root brls::Box will be treated as a view and added
     * as a child of the Box.
     */
    void inflateFromXMLElement(tinyxml2::XMLElement* element);

    /**
     * Inflates the Box with the given XML resource.
     *
     * The root element MUST be a brls::Box, corresponding to the inflated Box itself. Its
     * attributes will be applied to the Box.
     *
     * Each child node in the root brls::Box will be treated as a view and added
     * as a child of the Box.
     */
    void inflateFromXMLRes(std::string res);

    /**
     * Inflates the Box with the given XML file path.
     *
     * The root element MUST be a brls::Box, corresponding to the inflated Box itself. Its
     * attributes will be applied to the Box.
     *
     * Each child node in the root brls::Box will be treated as a view and added
     * as a child of the Box.
     */
    void inflateFromXMLFile(std::string path);

    /**
     * Handles a child XML element.
     *
     * By default, calls createFromXMLElement() and adds the result
     * to the children of the Box.
     */
    void handleXMLElement(tinyxml2::XMLElement* element) override;
};

// An empty view that has auto x auto and grow=1.0 to push
// all the next views in its box to the right (or to the bottom)
class Padding : public View
{
  public:
    Padding();

    void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) override;

    static View* create();
};

} // namespace brls
