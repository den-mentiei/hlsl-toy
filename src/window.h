#pragma once

#include "windows.h"

namespace toy {

class Window {
public:
	Window();
	~Window();

	void open(HINSTANCE instance, const wchar_t* title, const unsigned w, const unsigned h);
	void update();
	bool is_closing() const;

	HWND handle() const;

	static void register_class(HINSTANCE instance);
private:
	void close();

	static LRESULT WINAPI windows_proc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);
	void handle_close();

	HWND _handle;
	bool _is_closing;
};

} // namespace toy