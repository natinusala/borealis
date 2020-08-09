/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
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

#include <borealis/application.hpp>
#include <borealis/box_layout.hpp>
#include <borealis/rectangle.hpp>
#include <borealis/sidebar.hpp>
#include <borealis/tab_frame.hpp>
#include <borealis/layer_view.hpp>

namespace brls
{

TabFrame::TabFrame()
    : AppletFrame(false, true)
{
    //Create sidebar
    this->sidebar = new Sidebar();

    // Setup content view
    this->layout = new BoxLayout(BoxLayoutOrientation::HORIZONTAL);
    layout->addView(sidebar);

    rightPane = new LayerView();

    layout->addView(rightPane, true, false);

    this->setContentView(layout);
}

bool TabFrame::onCancel()
{
    // Go back to sidebar if not already focused
    if (!this->sidebar->isFocused())
    {
        Application::onGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_LEFT, false);
        return true;
    }

    return AppletFrame::onCancel();
}

void TabFrame::switchToView(int viewIndex)
{
    if(this->rightPane->getLayerIndex() == viewIndex)
        return;

    this->rightPane->changeLayer(viewIndex);

    if (this->layout->getViewsCount() > 1)
    {
        if (this->rightPane)
            this->rightPane->willDisappear(true);
        this->layout->removeView(1, false);
    }
    
    if (this->rightPane != nullptr)
        this->layout->addView(this->rightPane, true, true); // addView() calls willAppear()
}

void TabFrame::addTab(std::string label, View* view)
{

    size_t viewIndex        = rightPane->getLayerCount();
    size_t* viewUserData = (size_t*)malloc(sizeof(size_t));
    *viewUserData        = viewIndex;

    rightPane->addLayer(view);

    ListItem* item = this->sidebar->addItem(label, viewUserData);

    item->getFocusEvent()->subscribe([this](View* view) {
        if (ListItem* item = dynamic_cast<ListItem*>(view))
        {
            int viewIndex = static_cast<int>(*((size_t*)item->getAssociatedViewUserData()));
            this->switchToView(viewIndex);
        }
    });

    // Switch to first one as soon as we add it
    if (!this->rightPane)
    {
        Logger::debug("Switching to the first tab");
        this->switchToView(0);
    }
}

void TabFrame::addSeparator()
{
    this->sidebar->addSeparator();
}

View* TabFrame::getDefaultFocus()
{
    // Try to focus the right pane
    if (this->layout->getViewsCount() > 1)
    {
        View* newFocus = this->rightPane->getDefaultFocus();

        if (newFocus)
            return newFocus;
    }

    // Otherwise focus sidebar
    return this->sidebar->getDefaultFocus();
}

TabFrame::~TabFrame()
{
    switchToView(-1);

    // Content view is freed by ~AppletFrame()
}

} // namespace brls
