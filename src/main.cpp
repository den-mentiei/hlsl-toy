#include "windows.h"
#include "application.h"
#include "options.h"

using namespace toy;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	Options options;

	Application app;
	if (!app.init(hInstance, options)) {
		return 1;
	}
	while (app.work()) {};

	return 0;
}