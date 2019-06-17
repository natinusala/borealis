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
    // Init the app
    if (!Application::init(StyleEnum::ACCURATE))
    {
        printf("Unable to init Borealis application\n");
        return EXIT_FAILURE;
    }

    // Create a sample view
    TabFrame *rootFrame = new TabFrame();
    rootFrame->setTitle("Borealis Example App");

    List *testList = new List();
    ListItem *themeItem = new ListItem("TV Resolution");
    themeItem->setValue("Automatic");

    ListItem *jankItem = new ListItem("User Interface Jank", "Some people believe homebrews to have a jank user interface. Set to Minimal to have a native look and feel, set to Maximal to have a SX OS look and feel.");
    jankItem->setValue("Minimal");

    ListItem *crashItem = new ListItem("Divide by 0", "Can the Switch do it?");
    crashItem->setClickListener([](View *view){ Application::crash("The software was closed because an error occured:\nSIGABRT (signal 6)"); });

    testList->addView(themeItem);
    testList->addView(jankItem);
    testList->addView(crashItem);

    Label *testLabel = new Label(LabelStyle::REGULAR, "For more information about how to use Nintendo Switch and its features, please refer to the Nintendo Support Website on your smart device or PC.", true);
    testList->addView(testLabel);

    rootFrame->addTab("First tab", testList);
    rootFrame->addTab("Second tab", new Rectangle(nvgRGB(0, 0, 255)));
    rootFrame->addSeparator();
    rootFrame->addTab("Third tab", new Rectangle(nvgRGB(255, 0, 0)));
    rootFrame->addTab("Fourth tab", new Rectangle(nvgRGB(0, 255, 0)));

    // Add the root view to the stack
    Application::pushView(rootFrame);

    // Run the app
    while (Application::mainLoop());

    // Exit
    return EXIT_SUCCESS;
}