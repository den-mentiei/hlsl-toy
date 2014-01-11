#pragma once

#include "windows.h"

namespace toy {

class Window {
public:
	static void register_class(HINSTANCE instance);

	Window();
	~Window();

	void open(HINSTANCE instance, const wchar_t* title, const unsigned w, const unsigned h);
	void update();
	bool is_closing() const;

	HWND handle() const;

	unsigned width() const;
	unsigned height() const;

	typedef void (*KeypressCallback)(unsigned key_code, void* userdata);
	void set_keypress_callback(KeypressCallback callback, void* userdata);
private:
	void close();

	static LRESULT WINAPI windows_proc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);
	void handle_close();
	void handle_key_down(unsigned key_code);

	KeypressCallback _keypress_cb;
	void* _keypress_cb_userdata;

	HWND _handle;
	unsigned _w, _h;
	bool _is_closing;
};

} // namespace toy