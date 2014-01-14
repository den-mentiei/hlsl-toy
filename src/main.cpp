#include "windows.h"
#include "application.h"

using namespace toy;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	int argc;
	LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);

	if (argc < 2) {
		return 1;
	}

	Application app;
	if (!app.init(hInstance, argv[1], 1280, 720)) {
		return 1;
	}
	while (app.work()) {};

	::LocalFree(argv);
	return 0;
}