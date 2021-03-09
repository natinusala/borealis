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

#include <tinyxml2.h>

#include <borealis/core/application.hpp>
#include <borealis/core/box.hpp>
#include <borealis/core/util.hpp>
#include <cmath>

namespace brls
{

static YGFlexDirection getYGFlexDirection(Axis axis)
{
    switch (axis)
    {
        default:
        case Axis::ROW:
            return YGFlexDirectionRow;
        case Axis::COLUMN:
            return YGFlexDirectionColumn;
    }
}

Box::Box(Axis axis)
    : axis(axis)
{
    YGNodeStyleSetFlexDirection(this->ygNode, getYGFlexDirection(axis));

    // no need to invalidate if the box is empty and is not attached to any parent

    // Register XML attributes
    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "axis", Axis, this->setAxis,
        {
            { "row", Axis::ROW },
            { "column", Axis::COLUMN },
        });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "direction", Direction, this->setDirection,
        {
            { "inherit", Direction::INHERIT },
            { "leftToRight", Direction::LEFT_TO_RIGHT },
            { "rightToLeft", Direction::RIGHT_TO_LEFT },
        });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "justifyContent", JustifyContent, this->setJustifyContent,
        {
            { "flexStart", JustifyContent::FLEX_START },
            { "center", JustifyContent::CENTER },
            { "flexEnd", JustifyContent::FLEX_END },
            { "spaceBetween", JustifyContent::SPACE_BETWEEN },
            { "spaceAround", JustifyContent::SPACE_AROUND },
            { "spaceEvenly", JustifyContent::SPACE_EVENLY },
        });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "alignItems", AlignItems, this->setAlignItems,
        {
            { "auto", AlignItems::AUTO },
            { "flexStart", AlignItems::FLEX_START },
            { "center", AlignItems::CENTER },
            { "flexEnd", AlignItems::FLEX_END },
            { "stretch", AlignItems::STRETCH },
            { "baseline", AlignItems::BASELINE },
            { "spaceBetween", AlignItems::SPACE_BETWEEN },
            { "spaceAround", AlignItems::SPACE_AROUND },
        });

    // Padding
    this->registerFloatXMLAttribute("paddingTop", [this](float value) {
        this->setPaddingTop(value);
    });

    this->registerFloatXMLAttribute("paddingRight", [this](float value) {
        this->setPaddingRight(value);
    });

    this->registerFloatXMLAttribute("paddingBottom", [this](float value) {
        this->setPaddingBottom(value);
    });

    this->registerFloatXMLAttribute("paddingLeft", [this](float value) {
        this->setPaddingLeft(value);
    });

    this->registerFloatXMLAttribute("padding", [this](float value) {
        this->setPadding(value);
    });
}

Box::Box()
    : Box(Axis::ROW)
{
    // Empty ctor for XML
}

void Box::getCullingBounds(float* top, float* right, float* bottom, float* left)
{
    *top    = this->getY();
    *left   = this->getX();
    *right  = *left + this->getWidth();
    *bottom = *top + this->getHeight();
}

void Box::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    for (View* child : this->children)
    {
        // Ensure that the child is in bounds of all parents before drawing it
        // Only do that check for leaf views, as nested boxes will do that check themselves
        if (!dynamic_cast<Box*>(child) && child->isCulled())
        {
            float childTop    = child->getY();
            float childLeft   = child->getX();
            float childRight  = childLeft + child->getWidth();
            float childBottom = childTop + child->getHeight();

            bool draw = true;

            for (Box* bounds = this; bounds != nullptr; bounds = bounds->getParent())
            {
                float top, right, bottom, left;
                bounds->getCullingBounds(&top, &right, &bottom, &left);

                if (
                    childBottom < top || // too high
                    childRight < left || // too far left
                    childLeft > right || // too far right
                    childTop > bottom // too low
                )
                {
                    draw = false;
                    break;
                }
            }

            if (!draw)
                continue;
        }

        child->frame(ctx);
    }
}

void Box::addView(View* view)
{
    size_t position = YGNodeGetChildCount(this->ygNode);
    this->addView(view, position);
}

void Box::addView(View* view, size_t position)
{
    // Add the view to our children and YGNode
    this->children.insert(this->children.begin() + position, view);

    if (!view->isDetached())
        YGNodeInsertChild(this->ygNode, view->getYGNode(), position);

    // Allocate and set parent userdata
    size_t* userdata = (size_t*)malloc(sizeof(size_t));
    *userdata        = position;

    view->setParent(this, userdata);

    // Layout and events
    this->invalidate();
    view->willAppear();
}

void Box::removeView(View* view)
{
    if (!view)
        return;

    // Find the index of the view
    size_t index;
    bool found = false;

    for (size_t i = 0; i < this->children.size(); i++)
    {
        View* child = this->children[i];

        if (child == view)
        {
            found = true;
            index = i;
            break;
        }
    }

    if (!found)
        return;

    // Remove it
    YGNodeRemoveChild(this->ygNode, view->getYGNode());
    this->children.erase(this->children.begin() + index);

    view->willDisappear(true);
    delete view;

    this->invalidate();
}

void Box::onFocusGained()
{
    View::onFocusGained();

    for (View* child : this->children)
        child->onParentFocusGained(this);
}

void Box::onFocusLost()
{
    View::onFocusLost();

    for (View* child : this->children)
        child->onParentFocusLost(this);
}

void Box::onParentFocusGained(View* focusedView)
{
    View::onParentFocusGained(focusedView);

    for (View* child : this->children)
        child->onParentFocusGained(focusedView);
}

void Box::onParentFocusLost(View* focusedView)
{
    View::onParentFocusLost(focusedView);

    for (View* child : this->children)
        child->onParentFocusLost(focusedView);
}

void Box::setPadding(float top, float right, float bottom, float left)
{
    YGNodeStyleSetPadding(this->ygNode, YGEdgeTop, top);
    YGNodeStyleSetPadding(this->ygNode, YGEdgeRight, right);
    YGNodeStyleSetPadding(this->ygNode, YGEdgeBottom, bottom);
    YGNodeStyleSetPadding(this->ygNode, YGEdgeLeft, left);

    this->invalidate();
}

void Box::setPadding(float padding)
{
    this->setPadding(padding, padding, padding, padding);
}

void Box::setPaddingTop(float top)
{
    YGNodeStyleSetPadding(this->ygNode, YGEdgeTop, top);
    this->invalidate();
}

void Box::setPaddingRight(float right)
{
    YGNodeStyleSetPadding(this->ygNode, YGEdgeRight, right);
    this->invalidate();
}

void Box::setPaddingBottom(float bottom)
{
    YGNodeStyleSetPadding(this->ygNode, YGEdgeBottom, bottom);
    this->invalidate();
}

void Box::setPaddingLeft(float left)
{
    YGNodeStyleSetPadding(this->ygNode, YGEdgeLeft, left);
    this->invalidate();
}

View* Box::getDefaultFocus()
{
    // Focus ourself first
    if (this->isFocusable())
        return this;

    // Then try default focus
    if (this->defaultFocusedIndex < this->children.size())
    {
        View* newFocus = this->children[this->defaultFocusedIndex]->getDefaultFocus();

        if (newFocus)
            return newFocus;
    }

    // Fallback to finding the first focusable view
    for (size_t i = 0; i < this->children.size(); i++)
    {
        View* newFocus = this->children[i]->getDefaultFocus();

        if (newFocus)
            return newFocus;
    }

    return nullptr;
}

View* Box::getNextFocus(FocusDirection direction, View* currentView)
{
    void* parentUserData = currentView->getParentUserData();

    // Return nullptr immediately if focus direction mismatches the box axis (clang-format refuses to split it in multiple lines...)
    if ((this->axis == Axis::ROW && direction != FocusDirection::LEFT && direction != FocusDirection::RIGHT) || (this->axis == Axis::COLUMN && direction != FocusDirection::UP && direction != FocusDirection::DOWN))
    {
        return nullptr;
    }

    // Traverse the children
    size_t offset = 1; // which way we are going in the children list

    if ((this->axis == Axis::ROW && direction == FocusDirection::LEFT) || (this->axis == Axis::COLUMN && direction == FocusDirection::UP))
    {
        offset = -1;
    }

    size_t currentFocusIndex = *((size_t*)parentUserData) + offset;
    View* currentFocus       = nullptr;

    while (!currentFocus && currentFocusIndex >= 0 && currentFocusIndex < this->children.size())
    {
        currentFocus = this->children[currentFocusIndex]->getDefaultFocus();
        currentFocusIndex += offset;
    }

    return currentFocus;
}

void Box::willAppear(bool resetState)
{
    for (View* child : this->children)
        child->willAppear(resetState);
}

void Box::willDisappear(bool resetState)
{
    for (View* child : this->children)
        child->willDisappear(resetState);
}

void Box::onWindowSizeChanged()
{
    for (View* child : this->children)
        child->onWindowSizeChanged();
}

std::vector<View*>& Box::getChildren()
{
    return this->children;
}

void Box::inflateFromXMLString(std::string xml)
{
    // Load XML
    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    tinyxml2::XMLError error        = document->Parse(xml.c_str());

    this->bindXMLDocument(document);

    if (error != tinyxml2::XMLError::XML_SUCCESS)
        fatal("Invalid XML when inflating " + this->describe() + ": error " + std::to_string(error));

    tinyxml2::XMLElement* element = document->RootElement();

    if (!element)
        fatal("Invalid XML: no element found");

    return Box::inflateFromXMLElement(element);
}

void Box::inflateFromXMLRes(std::string name)
{
    return Box::inflateFromXMLFile(std::string(BRLS_RESOURCES) + name);
}

void Box::inflateFromXMLFile(std::string path)
{
    // Load XML
    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    tinyxml2::XMLError error        = document->LoadFile(path.c_str());

    this->bindXMLDocument(document);

    if (error != tinyxml2::XMLError::XML_SUCCESS)
        fatal("Invalid XML when inflating " + this->describe() + ": error " + std::to_string(error));

    tinyxml2::XMLElement* element = document->RootElement();

    if (!element)
        fatal("Invalid XML: no element found");

    return Box::inflateFromXMLElement(element);
}

void Box::inflateFromXMLElement(tinyxml2::XMLElement* element)
{
    // Ensure element is a Box
    if (std::string(element->Name()) != "brls:Box")
        fatal("First XML element is " + std::string(element->Name()) + ", expected brls:Box");

    // Apply attributes
    this->applyXMLAttributes(element);

    // Handle children
    for (tinyxml2::XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
        this->addView(View::createFromXMLElement(child)); // don't call handleXMLElement because this method is for user XMLs
}

void Box::handleXMLElement(tinyxml2::XMLElement* element)
{
    this->addView(View::createFromXMLElement(element));
}

void Box::setAxis(Axis axis)
{
    YGNodeStyleSetFlexDirection(this->ygNode, getYGFlexDirection(axis));
    this->axis = axis;
    this->invalidate();
}

void Box::setDirection(Direction direction)
{
    switch (direction)
    {
        case Direction::INHERIT:
            YGNodeStyleSetDirection(this->ygNode, YGDirectionInherit);
            break;
        case Direction::LEFT_TO_RIGHT:
            YGNodeStyleSetDirection(this->ygNode, YGDirectionLTR);
            break;
        case Direction::RIGHT_TO_LEFT:
            YGNodeStyleSetDirection(this->ygNode, YGDirectionRTL);
            break;
    }

    this->invalidate();
}

void Box::setJustifyContent(JustifyContent justify)
{
    switch (justify)
    {
        case JustifyContent::FLEX_START:
            YGNodeStyleSetJustifyContent(this->ygNode, YGJustifyFlexStart);
            break;
        case JustifyContent::CENTER:
            YGNodeStyleSetJustifyContent(this->ygNode, YGJustifyCenter);
            break;
        case JustifyContent::FLEX_END:
            YGNodeStyleSetJustifyContent(this->ygNode, YGJustifyFlexEnd);
            break;
        case JustifyContent::SPACE_BETWEEN:
            YGNodeStyleSetJustifyContent(this->ygNode, YGJustifySpaceBetween);
            break;
        case JustifyContent::SPACE_AROUND:
            YGNodeStyleSetJustifyContent(this->ygNode, YGJustifySpaceAround);
            break;
        case JustifyContent::SPACE_EVENLY:
            YGNodeStyleSetJustifyContent(this->ygNode, YGJustifySpaceEvenly);
            break;
    }

    this->invalidate();
}

void Box::setAlignItems(AlignItems alignment)
{
    switch (alignment)
    {
        case AlignItems::AUTO:
            YGNodeStyleSetAlignItems(this->ygNode, YGAlignAuto);
            break;
        case AlignItems::FLEX_START:
            YGNodeStyleSetAlignItems(this->ygNode, YGAlignFlexStart);
            break;
        case AlignItems::CENTER:
            YGNodeStyleSetAlignItems(this->ygNode, YGAlignCenter);
            break;
        case AlignItems::FLEX_END:
            YGNodeStyleSetAlignItems(this->ygNode, YGAlignFlexEnd);
            break;
        case AlignItems::STRETCH:
            YGNodeStyleSetAlignItems(this->ygNode, YGAlignStretch);
            break;
        case AlignItems::BASELINE:
            YGNodeStyleSetAlignItems(this->ygNode, YGAlignBaseline);
            break;
        case AlignItems::SPACE_BETWEEN:
            YGNodeStyleSetAlignItems(this->ygNode, YGAlignSpaceBetween);
            break;
        case AlignItems::SPACE_AROUND:
            YGNodeStyleSetAlignItems(this->ygNode, YGAlignSpaceAround);
            break;
    }

    this->invalidate();
}

View* Box::getView(std::string id)
{
    if (id == this->id)
        return this;

    for (View* child : this->children)
    {
        View* result = child->getView(id);

        if (result)
            return result;
    }

    return nullptr;
}

bool Box::applyXMLAttribute(std::string name, std::string value)
{
    if (this->forwardedAttributes.count(name) > 0)
    {
        std::pair<std::string, View*> pair = this->forwardedAttributes[name];
        return pair.second->applyXMLAttribute(pair.first, value);
    }

    return View::applyXMLAttribute(name, value);
}

void Box::forwardXMLAttribute(std::string attributeName, View* target)
{
    this->forwardXMLAttribute(attributeName, target, attributeName);
}

void Box::forwardXMLAttribute(std::string attributeName, View* target, std::string targetAttributeName)
{
    if (!target->isXMLAttributeValid(targetAttributeName))
        fatal("Error when forwarding \"" + attributeName + "\" of \"" + this->describe() + "\": attribute \"" + targetAttributeName + "\" is not a XML valid attribute for view \"" + target->describe() + "\"");

    if (this->forwardedAttributes.count(attributeName) > 0)
        fatal("Error when forwarding \"" + attributeName + "\" of \"" + this->describe() + "\": the same attribute cannot be forwarded twice");

    this->forwardedAttributes[attributeName] = std::make_pair(targetAttributeName, target);
}

void Box::onChildFocusGained(View* directChild, View* focusedView)
{
    if (this->hasParent())
        this->getParent()->onChildFocusGained(this, focusedView);
}

void Box::onChildFocusLost(View* directChild, View* focusedView)
{
    if (this->hasParent())
        this->getParent()->onChildFocusLost(this, focusedView);
}

View* Box::create()
{
    return new Box();
}

Padding::Padding()
{
    this->setGrow(1.0f);
}

void Padding::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
}

View* Padding::create()
{
    return new Padding();
}

} // namespace brls
