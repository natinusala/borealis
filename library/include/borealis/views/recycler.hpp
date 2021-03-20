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
    /*
     * Cell's position inside recycler frame
     */
    int getIndexPath() const { return indexPath; }

    /*
     * DO NOT USE! FOR INTERNAL USAGE ONLY!
     */
    void setIndexPath(int value) { indexPath = value; }

    /*
     * A string used to identify a cell that is reusable.
     */
    std::string reuseIdentifier;

    /*
     * Prepares a reusable cell for reuse by the recycler frame's data source.
     */
    virtual void prepareForReuse() { }

    static RecyclerCell* create();

  private:
    int indexPath;
};

class RecyclerFrame;
class RecyclerDataSource
{
  public:
    virtual int numberOfRows() { return 0; }
    virtual RecyclerCell* cellForRow(RecyclerFrame* recycler, int row) { return nullptr; }

    /*
     * Used to provide row height.
     * Return -1 to use autoscaling.
     */
    virtual float cellHeightForRow(int row) { return -1; }
};

class RecyclerContentBox : public Box
{
  public:
    RecyclerContentBox();
    View* getNextFocus(FocusDirection direction, View* currentView) override;
};

// Custom Box for propper recycling navigation
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

    /*
     * Set an object that acts as the data source of the recycler frame.
     */
    void setDataSource(RecyclerDataSource* source);

    /*
     * Reloads the rows of the recycler frame.
     */
    void reloadData();

    /*
     * Registers a class for use in creating new recycler cells.
     */
    void registerCell(std::string identifier, std::function<RecyclerCell*(void)> allocation);

    /*
     * Returns a reusable recycler-frame cell object for the specified reuse identifier
     */
    RecyclerCell* dequeueReusableCell(std::string identifier);

    /*
     * Used for initial recycler's frame calculation if rows autoscaling selected.
     * To provide more accurate height implement DataSource->cellHeightForRow().
     */
    float estimatedRowHeight = 44;

    static View* create();

  private:
    RecyclerDataSource* dataSource = nullptr;

    uint32_t visibleMin, visibleMax;

    float paddingTop    = 0;
    float paddingRight  = 0;
    float paddingBottom = 0;
    float paddingLeft   = 0;

    Box* contentBox;
    Rect renderedFrame;
    std::vector<Size> cacheFramesData;
    std::map<std::string, std::vector<RecyclerCell*>*> queueMap;
    std::map<std::string, std::function<RecyclerCell*(void)>> allocationMap;

    bool checkWidth();

    void cacheCellFrames();
    void cellsRecyclingLoop();
    void queueReusableCell(RecyclerCell* cell);

    void addCellAt(int index, int downSide);
};

} // namespace brls
