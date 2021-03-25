/*
    Copyright 2021 XITRIX

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
#include <borealis/core/touch/tap_gesture.hpp>
#include <borealis/views/recycler.hpp>

namespace brls
{

RecyclerCell::RecyclerCell()
{
    this->registerClickAction([this](View* view) {
        RecyclerFrame* recycler = dynamic_cast<RecyclerFrame*>(getParent()->getParent());
        if (recycler)
            recycler->getDataSource()->didSelectRowAt(indexPath);
        return true;
    });

    this->addGestureRecognizer(new TapGestureRecognizer(this));
}

RecyclerCell* RecyclerCell::create()
{
    return new RecyclerCell();
}

RecyclerContentBox::RecyclerContentBox()
    : Box(Axis::COLUMN)
{
}

View* RecyclerContentBox::getNextFocus(FocusDirection direction, View* currentView)
{
    void* parentUserData = currentView->getParentUserData();

    // Return nullptr immediately if focus direction mismatches the box axis (clang-format refuses to split it in multiple lines...)
    if ((this->getAxis() == Axis::ROW && direction != FocusDirection::LEFT && direction != FocusDirection::RIGHT) || (this->getAxis() == Axis::COLUMN && direction != FocusDirection::UP && direction != FocusDirection::DOWN))
    {
        return nullptr;
    }

    // Traverse the children
    size_t offset = 1; // which way we are going in the children list

    if ((this->getAxis() == Axis::ROW && direction == FocusDirection::LEFT) || (this->getAxis() == Axis::COLUMN && direction == FocusDirection::UP))
    {
        offset = -1;
    }

    size_t currentFocusIndex = *((size_t*)parentUserData) + offset;
    View* currentFocus       = nullptr;

    for (auto it : getChildren())
    {
        if (*((size_t*)it->getParentUserData()) == currentFocusIndex)
            currentFocus = it->getDefaultFocus();
    }

    return currentFocus;
}

RecyclerFrame::RecyclerFrame()
{
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

    this->setScrollingBehavior(ScrollingBehavior::CENTERED);

    // Create content box
    this->contentBox = new RecyclerContentBox();
    this->setContentView(this->contentBox);
}

RecyclerFrame::~RecyclerFrame()
{
    if (this->dataSource)
        delete dataSource;

    for (auto it : queueMap)
        delete it.second;
}

void RecyclerFrame::setDataSource(RecyclerDataSource* source)
{
    if (this->dataSource)
        delete this->dataSource;

    this->dataSource = source;
    if (checkWidth())
        reloadData();
}

RecyclerDataSource* RecyclerFrame::getDataSource() const
{
    return this->dataSource;
}

void RecyclerFrame::reloadData()
{
    auto children = this->contentBox->getChildren();
    for (auto const& child : children)
    {
        queueReusableCell((RecyclerCell*)child);
        this->contentBox->removeView(child);
    }

    visibleMin = UINT_MAX;
    visibleMax = 0;

    renderedFrame            = Rect();
    renderedFrame.size.width = getWidth();

    setScroll(0);

    if (dataSource)
    {
        cacheCellFrames();
        Rect frame = getFrame();
        for (int i = 0; i < dataSource->numberOfRows(); i++)
        {
            addCellAt(i, true);
            if (renderedFrame.getMaxY() > frame.getMaxY())
                break;
        }
    }
}

void RecyclerFrame::registerCell(std::string identifier, std::function<RecyclerCell*()> allocation)
{
    queueMap.insert(std::make_pair(identifier, new std::vector<RecyclerCell*>()));
    allocationMap.insert(std::make_pair(identifier, allocation));
}

RecyclerCell* RecyclerFrame::dequeueReusableCell(std::string identifier)
{
    RecyclerCell* cell = nullptr;
    auto it            = queueMap.find(identifier);

    if (it != queueMap.end())
    {
        std::vector<RecyclerCell*>* vector = it->second;
        if (!vector->empty())
        {
            cell = vector->back();
            vector->pop_back();
        }
        else
        {
            cell                  = allocationMap.at(identifier)();
            cell->reuseIdentifier = identifier;
            cell->detach();
        }
    }

    if (cell)
        cell->prepareForReuse();

    return cell;
}

void RecyclerFrame::queueReusableCell(RecyclerCell* cell)
{
    queueMap.at(cell->reuseIdentifier)->push_back(cell);
}

void RecyclerFrame::cacheCellFrames()
{
    cacheFramesData.clear();
    Rect frame = getFrame();
    Point currentOrigin;

    if (dataSource)
    {
        for (int i = 0; i < dataSource->numberOfRows(); i++)
        {
            float height = dataSource->heightForRow(i);
            if (height == -1)
                height = estimatedRowHeight;

            cacheFramesData.push_back(Size(frame.getWidth(), height));
            currentOrigin.y += height;
        }
        contentBox->setHeight(currentOrigin.y + paddingTop + paddingBottom);
    }
}

bool RecyclerFrame::checkWidth()
{
    float width           = getWidth();
    static float oldWidth = width;
    if (oldWidth != width && width != 0)
    {
        oldWidth = width;
        return true;
    }
    oldWidth = width;
    return false;
}

void RecyclerFrame::cellsRecyclingLoop()
{
    Rect frame        = getFrame();
    Rect visibleFrame = getVisibleFrame();
    visibleFrame.origin.y -= frame.origin.y;

    while (true)
    {
        RecyclerCell* minCell = nullptr;
        for (auto it : contentBox->getChildren())
            if (*((size_t*)it->getParentUserData()) == visibleMin)
                minCell = (RecyclerCell*)it;

        if (!minCell || minCell->getDetachedPosition().y + minCell->getHeight() >= visibleFrame.getMinY())
            break;

        float cellHeight = minCell->getHeight();
        renderedFrame.origin.y += cellHeight;
        renderedFrame.size.height -= cellHeight;

        queueReusableCell(minCell);
        this->contentBox->removeView(minCell);

        Logger::debug("Cell #" + std::to_string(visibleMin) + " - destroyed");

        visibleMin++;
    }

    while (true)
    {
        RecyclerCell* maxCell = nullptr;
        for (auto it : contentBox->getChildren())
            if (*((size_t*)it->getParentUserData()) == visibleMax)
                maxCell = (RecyclerCell*)it;

        if (!maxCell || maxCell->getDetachedPosition().y <= visibleFrame.getMaxY())
            break;

        float cellHeight = maxCell->getHeight();
        renderedFrame.size.height -= cellHeight;

        queueReusableCell(maxCell);
        this->contentBox->removeView(maxCell);

        Logger::debug("Cell #" + std::to_string(visibleMax) + " - destroyed");

        visibleMax--;
    }

    while (visibleMin - 1 < cacheFramesData.size() && renderedFrame.getMinY() > visibleFrame.getMinY() - paddingTop)
    {
        int i = visibleMin - 1;
        addCellAt(i, false);
    }

    while (visibleMax + 1 < cacheFramesData.size() && renderedFrame.getMaxY() < visibleFrame.getMaxY() - paddingBottom)
    {
        int i = visibleMax + 1;
        addCellAt(i, true);
    }
}

void RecyclerFrame::addCellAt(int index, int downSide)
{
    RecyclerCell* cell = dataSource->cellForRow(this, index);
    cell->setWidth(renderedFrame.getWidth() - paddingLeft - paddingRight);
    Point cellOrigin = Point(renderedFrame.getMinX() + paddingLeft, (downSide ? renderedFrame.getMaxY() : renderedFrame.getMinY() - cell->getHeight()) + paddingTop);
    cell->setDetachedPosition(cellOrigin.x, cellOrigin.y);
    cell->setIndexPath(index);

    this->contentBox->getChildren().insert(this->contentBox->getChildren().end(), cell);

    // Allocate and set parent userdata
    size_t* userdata = (size_t*)malloc(sizeof(size_t));
    *userdata        = index;

    cell->setParent(this->contentBox, userdata);

    // Layout and events
    this->contentBox->invalidate();
    cell->View::willAppear();

    if (index < visibleMin)
        visibleMin = index;

    if (index > visibleMax)
        visibleMax = index;

    Rect cellFrame = cell->getFrame();

    if (!downSide)
        renderedFrame.origin.y -= cellFrame.getHeight();

    renderedFrame.size.height += cellFrame.getHeight();

    if (cellFrame.getHeight() != cacheFramesData[index].height)
    {
        float delta = cellFrame.getHeight() - cacheFramesData[index].height;
        contentBox->setHeight(contentBox->getHeight() + delta);
        cacheFramesData[index].height = cellFrame.getHeight();
    }

    Logger::debug("Cell #" + std::to_string(index) + " - added");
}

void RecyclerFrame::onLayout()
{
    ScrollingFrame::onLayout();
    this->contentBox->setWidth(this->getWidth());
    if (checkWidth())
        reloadData();
}

void RecyclerFrame::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    cellsRecyclingLoop();
    ScrollingFrame::draw(vg, x, y, width, height, style, ctx);
}

void RecyclerFrame::setPadding(float padding)
{
    this->setPadding(padding, padding, padding, padding);
}

void RecyclerFrame::setPadding(float top, float right, float bottom, float left)
{
    paddingTop    = top;
    paddingRight  = right;
    paddingBottom = bottom;
    paddingLeft   = left;

    this->reloadData();
}

void RecyclerFrame::setPaddingTop(float top)
{
    paddingTop = top;
    this->reloadData();
}

void RecyclerFrame::setPaddingRight(float right)
{
    paddingRight = right;
    this->reloadData();
}

void RecyclerFrame::setPaddingBottom(float bottom)
{
    paddingBottom = bottom;
    this->reloadData();
}

void RecyclerFrame::setPaddingLeft(float left)
{
    paddingLeft = left;
    this->reloadData();
}

View* RecyclerFrame::create()
{
    return new RecyclerFrame();
}

} // namespace brls
