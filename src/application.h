#pragma once

#include "windows.h"

#include "window.h"
#include "dx_render_device.h"

namespace toy {

class Application {
public:
	bool init(HINSTANCE instance);
	void shutdown();
	bool work();
private:
	HINSTANCE _instance;
	DXRenderDevice _render_device;
	Window _main_window;
};

} // namespace toy