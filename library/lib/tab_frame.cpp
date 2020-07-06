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

namespace brls
{

void ViewContainer::setView(View* view){
    //children.clear(); // Clear the only possible child.
    if(associated_view){
        associated_view->willDisappear(true);
    }
    if(view) {
        view->setParent(this);
        view->willAppear(true);
    }
    associated_view = view;
    invalidate();
}

void ViewContainer::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) {
    if(associated_view)
        associated_view->frame(ctx);
}


void ViewContainer::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    if(associated_view) {
        associated_view->setBoundaries(x, y, width, height);
        associated_view->invalidate();
    }
}

View* ViewContainer::getDefaultFocus() {
    if(associated_view)
        return associated_view->getDefaultFocus();
    return nullptr;
}

TabFrame::TabFrame()
    : AppletFrame(false, true)
{
    //Create sidebar
    this->sidebar = new Sidebar();

    // Create a ViewContainer that represents the right pane.
    this->rightPane = new ViewContainer();

    // Setup content view
    this->layout = new BoxLayout(BoxLayoutOrientation::HORIZONTAL);
    layout->addView(sidebar);
    layout->addView(rightPane, true, true);

    this->setContentView(layout);
}

bool TabFrame::onCancel()
{
    // Go back to sidebar if not already focused
    if (!this->sidebar->isChildFocused())
    {
        Application::onGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_LEFT, false);
        return true;
    }

    return AppletFrame::onCancel();
}

void TabFrame::addTab(std::string label, View* view)
{
    SidebarItem* item = this->sidebar->addItem(label, view);
    item->getFocusEvent()->subscribe([this](View* view) {
        if (SidebarItem* item = dynamic_cast<SidebarItem*>(view))
            this->rightPane->setView(item->getAssociatedView());
    });

    // Switch to first one as soon as we add it
    if (!this->rightPane->hasView())
    {
        Logger::debug("Switching to the first tab");
        rightPane->setView(view);
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
    rightPane->setView(nullptr);

    // Content view is freed by ~AppletFrame()
}

} // namespace brls
