#include "windows.h"
#include "application.h"

using namespace toy;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Application app;

	if (!app.init(hInstance)) {
		return 1;
	}
	while (app.work()) {};
	app.shutdown();

	return 0;
}