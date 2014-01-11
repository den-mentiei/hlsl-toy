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

	__declspec(align(16))
	struct ToyParameters {
		float time;
	};
	ToyParameters _toy_parameters;
	unsigned _toy_parameters_buffer;

	unsigned _vertices;
	unsigned _indices;
};

} // namespace toy