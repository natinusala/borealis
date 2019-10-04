/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  WerWolv
    Copyright (C) 2019  p-sam

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <borealis/view.hpp>
#include <vector>

namespace brls
{

enum class BoxLayoutOrientation
{
    VERTICAL = 0,
    HORIZONTAL
};

class BoxLayoutChild
{
  public:
    View* view;
    bool fill; // should the child fill the remaining space?
};

// A basic horizontal or vertical box layout :
// - Children can currently only be stretched to full width (vertical) or height (horizontal)
// - Only works with children with fixed width (horizontal) or height (vertical)
// - Handles vertical scrolling only

// TODO: Allow disabling scrolling
// TODO: Add alignment or stretching parameters to children
class BoxLayout : public View
{
  private:
    BoxLayoutOrientation orientation;

    unsigned spacing = 0;

    unsigned middleY = 0; // y + height/2
    unsigned bottomY = 0; // y + height
    unsigned rightX  = 0; // x + width

    unsigned entriesHeight = 0; // sum of all entries heights (with spacing) + bottom margin
    unsigned entriesWidth  = 0; // sum of all entries heights (with spacing) + bottom margin

    float scrollY = 0.0f; // all childrens are offset by this value
    float scrollX = 0.0f; // all childrens are offset by this value

    View* updateFocus(FocusDirection direction, View* oldFocus, bool fromUp);
    void updateScroll(bool animated = true);

    void scrollAnimationTick();
    void prebakeScrolling();

    bool firstLayout = true;

  protected:
    std::vector<BoxLayoutChild*> children;

    size_t defaultFocusedIndex = 0;
    size_t focusedIndex        = 0;

    unsigned marginTop    = 0;
    unsigned marginRight  = 0;
    unsigned marginBottom = 0;
    unsigned marginLeft   = 0;

    /**
      * Should the BoxLayout apply spacing after
      * this view?
      */
    virtual void customSpacing(View* current, View* next, int* spacing) {}

    virtual View* defaultFocus(View* oldFocus);

  public:
    BoxLayout(BoxLayoutOrientation orientation, size_t defaultFocus = 0);

    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    View* requestFocus(FocusDirection direction, View* oldFocus, bool fromUp = false) override;
    void willAppear() override;
    void willDisappear() override;

    /**
      * Sets spacing between views
      */
    void setSpacing(unsigned spacing);
    unsigned getSpacing();

    /**
      * Sets margins around views
      * Bottom (vertical) or right (horizontal) are
      * only effective if the last child is set to fill
      */
    void setMargins(unsigned top, unsigned right, unsigned bottom, unsigned left);
    void setMarginBottom(unsigned bottom);

    /**
      * Adds a view to this box layout
      * If fill is set to true, the child will
      * fill the remaining space
      */
    void addView(View* view, bool fill = false);

    /**
      * Removes the view at specified
      * The view will be freed if free
      * is set to true (defaults to true)
      *
      * Warning: this method isn't correctly
      * implemented - currently removing a view will
      * most likely result in memory corruption
      */
    void removeView(int index, bool free = true);

    /**
     * Removes all views
     * from this layout
     */
    void clear(bool free = true);

    /**
      * Returns true if this layout
      * doesn't contain any views
      */
    bool isEmpty();

    bool isChildFocused();

    void setFocusedIndex(unsigned index);
    size_t getViewsCount();

    ~BoxLayout();
};

} // namespace brls
