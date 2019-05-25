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

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <Borealis.hpp>

using namespace std;

int main(int argc, char* argv[])
{
    // Create and init the app
    Application *app = new Application(STYLE_ACCURATE);

    if (!app->init())
    {
        printf("Unable to init Borealis application");
        delete app;
        return EXIT_FAILURE;
    }

    // Create a sample view
    TabFrame *rootFrame = new TabFrame();
    rootFrame->setTitle("Borealis Example App");

    Sidebar *testSidebar = new Sidebar();
    testSidebar->addItem("Dummy 1", nullptr);
    testSidebar->addItem("Dummy 2", nullptr);
    testSidebar->addItem("Dummy 3", nullptr);

    Sidebar *testSidebar2 = new Sidebar();
    testSidebar2->addItem("Sans", nullptr);
    testSidebar2->addItem("Papyrus", nullptr);
    testSidebar2->addItem("Undyne", nullptr);

    rootFrame->addTab("First tab", testSidebar);
    rootFrame->addTab("Second tab", testSidebar2);
    rootFrame->addSeparator();
    rootFrame->addTab("Third tab", new Rectangle(nvgRGB(255, 0, 0)));
    rootFrame->addTab("Fourth tab",new Rectangle(nvgRGB(0, 255, 0)));
    rootFrame->addSeparator();
    rootFrame->addTab("Tab 5", nullptr);
    rootFrame->addTab("Tab 6", nullptr);
    rootFrame->addSeparator();
    rootFrame->addTab("Tab 7", nullptr);
    rootFrame->addTab("Tab 8", nullptr);
    rootFrame->addSeparator();
    rootFrame->addTab("Tab 9", nullptr);
    rootFrame->addTab("Tab 10", nullptr);

    // Add the root view to the stack
    app->pushView(rootFrame);

    // Run the app
    while (app->mainLoop());

    // Exit
    delete app;
    return EXIT_SUCCESS;
}