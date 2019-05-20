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
    Application *app = new Application();

    if (!app->init())
    {
        printf("Unable to init Borealis application");
        delete app;
        return EXIT_FAILURE;
    }

    // Create a sample view
    TabFrame *rootFrame = new TabFrame();
    rootFrame->setTitle("Borealis Example App");

    // Add the root view to the stack
    app->pushView(rootFrame);

    // Run the app
    while (app->mainLoop());

    // Exit
    delete app;
    return EXIT_SUCCESS;
}