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