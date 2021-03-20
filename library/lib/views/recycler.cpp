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
#include <borealis/views/recycler.hpp>

namespace brls
{

RecyclerCell* RecyclerCell::create()
{
    return new RecyclerCell();
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
    this->contentBox = new Box(Axis::COLUMN);
    this->contentBox->setPadding(100,100,100,100);
    this->setContentView(this->contentBox);
}

void RecyclerFrame::onLayout()
{
    ScrollingFrame::onLayout();
    if (checkWidth())
        reloadData();
}

void RecyclerFrame::setDataSource(RecyclerDataSource* source)
{
    this->dataSource = source;
    if (checkWidth())
        reloadData();
}

void RecyclerFrame::reloadData()
{
    auto children = this->contentBox->getChildren();
    for (auto const& child : children)
    {
        queueReusableCell((RecyclerCell*)child);
        this->contentBox->removeView(child);
    }

    visibleCells.clear();
    visibleMin = UINT_MAX;
    visibleMax = 0;
    
    renderedFrame = Rect();
    renderedFrame.size.width = getWidth();
    
    setScroll(0);

    if (dataSource)
    {
        cacheCellFrames();
        Rect frame = getFrame();
        for (int i = 0; i < dataSource->numberOfRows(); i++)
        {
            addCellAt(i);
            if (renderedFrame.getMaxY() > frame.getMaxY())
                break;
        }
    }
}

void RecyclerFrame::registerCell(std::string identifier, std::function<RecyclerCell*()> allocation)
{
    queueMap.insert(std::make_pair(identifier, new std::vector<RecyclerCell*>()));
    registerMap.insert(std::make_pair(identifier, allocation));
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
            cell             = registerMap.at(identifier)();
            cell->identifier = identifier;
            cell->detach();
        }
    }

    return cell;
}

void RecyclerFrame::queueReusableCell(RecyclerCell* cell)
{
    queueMap.at(cell->identifier)->push_back(cell);
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
            float height = dataSource->cellHeightForRow(i);
            if (height == -1)
                height = estimatedCellHeight;
            
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

void RecyclerFrame::cellRecycling()
{
    Rect frame        = getFrame();
    Rect visibleFrame = getVisibleFrame();
    visibleFrame.origin.y -= frame.origin.y;

    while (true)
    {
        auto minCell = visibleCells.find(visibleMin);
        if (minCell == visibleCells.end() || minCell->second->getDetachedPosition().y + minCell->second->getHeight() >= visibleFrame.getMinY())
            break;
        
        float cellHeight = minCell->second->getHeight();
        renderedFrame.origin.y += cellHeight;
        renderedFrame.size.height -= cellHeight;
         
        queueReusableCell(minCell->second);
        this->contentBox->removeView(minCell->second);
        visibleCells.erase(minCell->second->indexPath);
        
        visibleMin++;
    }
    
    while (true)
    {
        auto maxCell = visibleCells.find(visibleMax);
        if (maxCell == visibleCells.end() || maxCell->second->getDetachedPosition().y <= visibleFrame.getMaxY())
            break;
        
        float cellHeight = maxCell->second->getHeight();
        renderedFrame.size.height -= cellHeight;
        
        queueReusableCell(maxCell->second);
        this->contentBox->removeView(maxCell->second);
        visibleCells.erase(maxCell->second->indexPath);
        
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
    cell->setMaxWidth(renderedFrame.getWidth() - paddingLeft - paddingRight);
    Point cellOrigin = Point(renderedFrame.getMinX() + paddingLeft, (downSide ? renderedFrame.getMaxY() : renderedFrame.getMinY() - cell->getHeight()) + paddingTop);
    cell->setDetachedPosition(cellOrigin.x, cellOrigin.y);
    cell->indexPath = index;
    this->contentBox->addView(cell);

    visibleCells.insert(std::make_pair(index, cell));

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
}

void RecyclerFrame::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    cellRecycling();
    ScrollingFrame::draw(vg, x, y, width, height, style, ctx);
}

void RecyclerFrame::setPadding(float padding)
{
    this->setPadding(padding, padding, padding, padding);
}

void RecyclerFrame::setPadding(float top, float right, float bottom, float left)
{
    paddingTop = top;
    paddingRight = right;
    paddingBottom = bottom;
    paddingLeft = left;
    
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
