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

    List *testList = new List();
    ListItem *themeItem = new ListItem("Theme");
    themeItem->setValue("Basic White");

    ListItem *jankItem = new ListItem("User Interface Jank", "Some people believe homebrews to have a jank user interface. Set to Minimal to have a native look and feel, set to Maximal to have a SX OS look and feel.");
    jankItem->setValue("Minimal");

    testList->addView(themeItem);
    testList->addView(jankItem);

    Label *testLabel = new Label(LABELSTYLE_REGULAR, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.", true);
    testList->addView(testLabel);

    rootFrame->addTab("First tab", testList);
    rootFrame->addTab("Second tab", new Rectangle(nvgRGB(0, 0, 255)));
    rootFrame->addSeparator();
    rootFrame->addTab("Third tab", new Rectangle(nvgRGB(255, 0, 0)));
    rootFrame->addTab("Fourth tab", new Rectangle(nvgRGB(0, 255, 0)));

    // Add the root view to the stack
    app->pushView(rootFrame);

    // Run the app
    while (app->mainLoop());

    // Exit
    delete app;
    return EXIT_SUCCESS;
}