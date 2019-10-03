/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

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

// TODO: Add a "can be cancelled with B" flag
// TODO: Add buttons at creation time
// TODO: Add the blurred dialog type once the blur is finished

// A modal dialog with zero to three buttons
// and anything as content
// Use the Dialog::openWith... methods
class Dialog : public View
{
  private:
    Dialog(View* contentView);
    ~Dialog();

    View* contentView = nullptr;

    unsigned frameX, frameY, frameWidth, frameHeight;

  public:
    static void openWithView(View* contentView);
    static void openWithText(std::string text);

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;

    bool isTranslucent() override
    {
        return true;
    }
};

} // namespace brls
