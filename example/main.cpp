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

using namespace bo;
using namespace std;

int main(int argc, char* argv[])
{
	Application *app = new Application();

	if (!app->init())
	{
		printf("Unable to init Borealis application");
		delete app;
		return EXIT_FAILURE;
	}

	while (app->mainLoop());

	delete app;
	return EXIT_SUCCESS;
}