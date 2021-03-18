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
    Style style = Application::getStyle();

    this->setScrollingBehavior(ScrollingBehavior::CENTERED);

    // Create content box
    this->contentBox = new Box(Axis::COLUMN);

    this->contentBox->setPadding(
        style["brls/sidebar/padding_top"],
        style["brls/sidebar/padding_right"],
        style["brls/sidebar/padding_bottom"],
        style["brls/sidebar/padding_left"]);

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

    if (dataSource)
    {
        cacheCellFrames();
        for (int i = 0; i < dataSource->numberOfRows(); i++)
        {
            RecyclerCell* cell = dataSource->cellForRow(this, i);
            cell->setMaxWidth(this->getWidth());
            auto frame = cacheFramesData.at(i);
            cell->setDetachedPosition(frame.getMinX(), frame.getMinY());
            this->contentBox->addView(cell, i);
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
    Point currentOrigin;
    if (dataSource)
    {
        for (int i = 0; i < dataSource->numberOfRows(); i++)
        {
            RecyclerCell* cell = dataSource->cellForRow(this, i);
            float a            = this->getWidth();
            cell->setMaxWidth(a);
            Rect frame   = cell->getFrame();
            frame.origin = currentOrigin;
            cacheFramesData.push_back(frame);
            currentOrigin.y += frame.getHeight();
            queueReusableCell(cell);
        }
        contentBox->setHeight(currentOrigin.y);
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

void RecyclerFrame::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    ScrollingFrame::draw(vg, x, y, width, height, style, ctx);
}

View* RecyclerFrame::create()
{
    return new RecyclerFrame();
}

} // namespace brls
