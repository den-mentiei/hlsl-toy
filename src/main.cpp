#include "windows.h"
#include "application.h"

using namespace toy;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Application app;

	app.init(hInstance);
	while (app.work()) {};
	app.shutdown();

	return 0;
}