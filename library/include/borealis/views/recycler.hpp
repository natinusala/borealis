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

#pragma once

#include <borealis/core/bind.hpp>
#include <borealis/views/label.hpp>
#include <borealis/views/rectangle.hpp>
#include <borealis/views/scrolling_frame.hpp>
#include <functional>
#include <map>
#include <vector>

namespace brls
{

class RecyclerCell : public Box
{
  public:
    int indexPath;
    std::string identifier;
    static RecyclerCell* create();
};

class RecyclerFrame;
class RecyclerDataSource
{
  public:
    virtual int numberOfRows() { return 0; }
    virtual RecyclerCell* cellForRow(RecyclerFrame* recycler, int row) { return nullptr; }
    virtual float cellHeightForRow(int row) { return -1; }
};

class RecyclerFrame : public ScrollingFrame
{
  public:
    RecyclerFrame();
    
    void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) override;
    void onLayout() override;
    void setPadding(float padding) override;
    void setPadding(float top, float right, float bottom, float left) override;
    void setPaddingTop(float top) override;
    void setPaddingRight(float right) override;
    void setPaddingBottom(float bottom) override;
    void setPaddingLeft(float left) override;
    
    void setDataSource(RecyclerDataSource* source);

    void reloadData();
    void registerCell(std::string identifier, std::function<RecyclerCell*(void)> allocation);
    RecyclerCell* dequeueReusableCell(std::string identifier);
    
    // Used for initial recycler's frame calculation
    float estimatedCellHeight = 44;

    static View* create();

  private:
    RecyclerDataSource* dataSource = nullptr;
    
    
    Box* contentBox;
    Rect renderedFrame;
    std::vector<Size> cacheFramesData;
    std::map<int, RecyclerCell*> visibleCells;
    std::map<std::string, std::vector<RecyclerCell*>*> queueMap;
    std::map<std::string, std::function<RecyclerCell*(void)>> registerMap;

    bool checkWidth();

    void cellRecycling();
    void cacheCellFrames();
    void queueReusableCell(RecyclerCell* cell);
    
    void addCellAt(int index, int downSide = true);

    uint32_t visibleMin, visibleMax;
    
    float paddingTop = 0;
    float paddingRight = 0;
    float paddingBottom = 0;
    float paddingLeft = 0;
};

} // namespace brls
