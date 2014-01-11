#pragma once

#include "windows.h"
#include "window.h"

namespace toy {

class Application {
public:
	void init(HINSTANCE instance);
	void shutdown();
	bool work();
private:
	HINSTANCE _instance;
	Window _main_window;
};

} // namespace toy