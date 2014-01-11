#pragma once

#include "windows.h"

#include "window.h"
#include "dx_render_device.h"
#include "toy_file.h"

namespace toy {

class Application {
public:
	bool init(HINSTANCE instance, const wchar_t* toy_path);
	bool work();
private:
	static void on_keypress_callback(const unsigned key_code, void* userdata);

	void handle_keypress(const unsigned key_code);

	bool load_toy(const wchar_t* path);
	void create_scene();

	HINSTANCE _instance;
	DXRenderDevice _render_device;
	Window _main_window;

	__declspec(align(16))
	struct ToyParameters {
		float time;
	};
	ToyParameters _toy_parameters;

	DXRenderDevice::Batch _triangles;
	ToyFile _toy;
};

} // namespace toy