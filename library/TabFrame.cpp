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

#include <TabFrame.hpp>
#include <BoxLayout.hpp>
#include <Rectangle.hpp>
#include <Sidebar.hpp>
#include <Application.hpp>

TabFrame::TabFrame() : SettingsFrame(false, true)
{
    //Create sidebar
    this->sidebar = new Sidebar();

    // Setup content view
    this->layout = new BoxLayout(BoxLayoutOrientation::HORIZONTAL);
    layout->addView(sidebar);

    this->setContentView(layout);
}

bool TabFrame::onCancel()
{
    Application::onGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
    return true;
}

void TabFrame::switchToView(View *view)
{
    if (this->rightPane == view)
        return;

    if (this->layout->getViewsCount() > 1)
    {
        if (this->rightPane)
            this->rightPane->willDisappear();
        this->layout->removeView(1, false);
    }

    if (view != nullptr)
    {
        this->layout->addView(view, true);
        this->rightPane = view;
    }
}

void TabFrame::addTab(string label, View *view)
{
    SidebarItem *item = this->sidebar->addItem(label, view);
    item->setFocusListener([this](View *view) {
        if (SidebarItem *item = dynamic_cast<SidebarItem*>(view))
            this->switchToView(item->getAssociatedView());
    });

    // Switch to first tab
    if (view != nullptr && this->layout->getViewsCount() == 1)
        this->switchToView(view);
}

void TabFrame::addSeparator()
{
    this->sidebar->addSeparator();
}

View* TabFrame::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    if (fromUp)
        return View::requestFocus(direction, oldFocus);

    // Give focus to the right panel
    if (direction == FocusDirection::NONE && this->layout->getViewsCount() > 1)
    {
        View *newFocus = this->rightPane->requestFocus(direction, oldFocus);
        if (newFocus)
        {
            this->layout->setFocusedIndex(1);
            return newFocus;
        }
    }

    // Let the layout do its thing
    return this->layout->requestFocus(direction, oldFocus, fromUp);
}

TabFrame::~TabFrame()
{
    switchToView(nullptr);

    // Content view is freed by ~SettingsFrame()
}