#pragma once

#include "windows.h"

#include "window.h"
#include "dx_render_device.h"
#include "toy_file.h"
#include "timer.h"

namespace toy {

class Application {
public:
	bool init(HINSTANCE instance, const wchar_t* toy_path);
	bool work();
private:
	static void on_resize_callback(const unsigned w, const unsigned h, void *userdata);
	static void on_keypress_callback(const unsigned key_code, void* userdata);
	static void on_mouse_move_callback(const unsigned x, const unsigned y, Mouse::Button button, void* userdata);
	static void on_mouse_down_callback(const unsigned x, const unsigned y, Mouse::Button button, void* userdata);
	static void on_mouse_up_callback(const unsigned x, const unsigned y, Mouse::Button button, void* userdata);	

	void handle_resize(const unsigned w, const unsigned h);
	void handle_keypress(const unsigned key_code);
	void handle_mouse_move(const unsigned x, const unsigned y, Mouse::Button button);
	void handle_mouse_down(const unsigned x, const unsigned y, Mouse::Button button);
	void handle_mouse_up(const unsigned x, const unsigned y, Mouse::Button button);

	void reload();

	bool load_toy(const wchar_t* path);
	void create_scene();
	
	void update();
	void render();

	void update_toy_parameters();

	Timer _timer;

	HINSTANCE _instance;
	DXRenderDevice _render_device;
	Window _main_window;

	__declspec(align(16))
	struct ToyParameters {
		Float4 mouse;
		Float2 resolution;
		float time;
	};
	ToyParameters _toy_parameters;

	DXRenderDevice::Batch _triangles;
	ToyFile _toy;
	const wchar_t* _toy_path;
};

} // namespace toy