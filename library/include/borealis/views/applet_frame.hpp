/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2021  natinusala
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

#include <borealis/views/box.hpp>
#include <borealis/views/image.hpp>
#include <borealis/views/label.hpp>

namespace brls
{

// A Horizon settings-like frame, with header and footer (no sidebar)
class AppletFrame : public Box
{
  public:
    AppletFrame();

    void handleXMLElement(tinyxml2::XMLElement* element) override;

    /**
     * Sets the content view for that AppletFrame.
     * Will be placed between header and footer and expanded with grow factor
     * and width / height to AUTO.
     */
    void setContentView(View* view);

    void setTitle(std::string title);

    void setIconFromRes(std::string name);
    void setIconFromFile(std::string path);

    static View* create();

  private:
    Label* title;
    Image* icon;

  protected:
    View* contentView = nullptr;
};

} // namespace brls
