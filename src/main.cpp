#include "windows.h"
#include "application.h"

using namespace toy;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	int argc;
	LPWSTR* argv = ::CommandLineToArgvW(lpCmdLine, &argc);

	if (argc < 1) {
		return 1;
	}

	Application app;
	if (!app.init(hInstance, argv[0])) {
		return 1;
	}
	while (app.work()) {};

	::LocalFree(argv);
	return 0;
}