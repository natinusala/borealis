/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  natinusala

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

#include <borealis/view.hpp>

namespace brls
{

// TODO: horizontal scrolling

// A view that automatically scrolls vertically
// when one of its children gains focus
class ScrollView : public View
{
    private:
        View* contentView = nullptr;

    public:
        ~ScrollView();

        void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
        void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
        void willAppear() override;
        void willDisappear() override;
        View* getDefaultFocus() override;

        void setContentView(View* view);
        View* getContentView();
};

} // namespace brls

