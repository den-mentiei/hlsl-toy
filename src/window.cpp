#include "window.h"

namespace toy {

namespace {
	static const wchar_t* WINDOW_CLASS = L"hlsl-toy-class";
} // anonymous namespace

LRESULT WINAPI Window::windows_proc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
	Window* window = reinterpret_cast<Window*>(::GetWindowLong(handle, GWL_USERDATA));

	if (window == nullptr || window->is_closing()) {
		return DefWindowProcW(handle, message, wparam, lparam);
	}
	
	switch (message) {
		case WM_CLOSE:
			window->handle_close();
			break;

		case WM_KEYDOWN:
			window->handle_key_down(static_cast<unsigned>(wparam));
			break;

		case WM_LBUTTONDOWN:
			window->handle_mouse_down(unsigned(lparam & 0xFFFF), unsigned(lparam >> 16), Mouse::B_LEFT);
			break;

		case WM_RBUTTONDOWN:
			window->handle_mouse_down(unsigned(lparam & 0xFFFF), unsigned(lparam >> 16), Mouse::B_RIGHT);
			break;

		case WM_LBUTTONUP:
			window->handle_mouse_up(unsigned(lparam & 0xFFFF), unsigned(lparam >> 16), Mouse::B_LEFT);
			break;

		case WM_RBUTTONUP:
			window->handle_mouse_up(unsigned(lparam & 0xFFFF), unsigned(lparam >> 16), Mouse::B_RIGHT);
			break;

		case WM_MOUSEMOVE:
			window->handle_mouse_move(unsigned(lparam & 0xFFFF), unsigned(lparam >> 16), wparam);
			break;

		case WM_SIZE:
			window->handle_resize(unsigned(lparam & 0xFFFF), unsigned(lparam >> 16));
			break;

		default:
			return DefWindowProcW(handle, message, wparam, lparam);
	}
	
	return 0;
}

void Window::register_class(HINSTANCE instance) {
	WNDCLASSW window_class;

	window_class.style = 0;
	window_class.lpfnWndProc = Window::windows_proc;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = instance;
	window_class.hIcon = 0;
	window_class.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground = 0;
	window_class.lpszMenuName = 0;
	window_class.lpszClassName = WINDOW_CLASS;

	::RegisterClassW(&window_class);
}

Window::Window()
	: _handle(0)
	, _is_closing(false)
	, _w(0)
	, _h(0)
{}

Window::~Window() {
	if (_handle) {
		close();
	}
}

void Window::open(HINSTANCE instance, const wchar_t* title, const unsigned w, const unsigned h) {
	RECT desktop_rect;
	::GetClientRect(::GetDesktopWindow(), &desktop_rect);
	unsigned x = (desktop_rect.right - w) / 2;
	unsigned y = (desktop_rect.bottom - h) / 2;

	_w = w;
	_h = h;

	RECT window_rect;
	window_rect.left = x;
	window_rect.right = window_rect.left + w;
	window_rect.top = y;
	window_rect.bottom = window_rect.top + h;
	::AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, 0);

	_handle = ::CreateWindowW(WINDOW_CLASS, title, WS_OVERLAPPEDWINDOW, window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, 0, 0, instance, 0);
	::SetWindowLong(_handle, GWL_USERDATA, reinterpret_cast<LONG>(this));

	::ShowWindow(_handle, SW_SHOW);
	::UpdateWindow(_handle);
}

void Window::close() {
	::SetWindowLong(_handle, GWL_USERDATA, 0);
	::DestroyWindow(_handle);
	_handle = 0;
}

void Window::update() {
	MSG message;
	while (::PeekMessage(&message, _handle, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}

bool Window::is_closing() const {
	return _is_closing;
}

HWND Window::handle() const {
	return _handle;
}

void Window::handle_close() {
	_is_closing = true;
}

void Window::handle_resize(const unsigned w, const unsigned h) {
	_w = w;
	_h = h;

	if (_resize_cb) {
		_resize_cb(w, h, _resize_cb_userdata);
	}
}

void Window::handle_key_down(const unsigned key_code) {
	if (_keypress_cb) {
		_keypress_cb(key_code, _keypress_cb_userdata);
	}
}

void Window::handle_mouse_move(const unsigned x, const unsigned y, unsigned state) {
	if (_mouse_move_cb) {
		Mouse::Button button;
		switch (state) {
			case MK_LBUTTON:
				button = Mouse::B_LEFT;
				break;

			case MK_RBUTTON:
				button = Mouse::B_RIGHT;
				break;

			default:
				button = Mouse::B_NONE;
				break;
		}
		_mouse_move_cb(x, y, button, _mouse_move_cb_userdata);
	}
}

void Window::handle_mouse_down(const unsigned x, const unsigned y, Mouse::Button button) {
	if (_mouse_down_cb) {
		_mouse_down_cb(x, y, button, _mouse_down_cb_userdata);
	}
}

void Window::handle_mouse_up(const unsigned x, const unsigned y, Mouse::Button button) {
	if (_mouse_up_cb) {
		_mouse_up_cb(x, y, button, _mouse_up_cb_userdata);
	}
}

unsigned Window::width() const {
	return _w;
}

unsigned Window::height() const {
	return _h;
}

void Window::set_keypress_callback(KeypressCallback callback, void* userdata) {
	_keypress_cb = callback;
	_keypress_cb_userdata = userdata;
}

void Window::set_mouse_move_callback(MouseCallback callback, void* userdata) {
	_mouse_move_cb = callback;
	_mouse_move_cb_userdata = userdata;
}

void Window::set_mouse_down_callback(MouseCallback callback, void* userdata) {
	_mouse_down_cb = callback;
	_mouse_down_cb_userdata = userdata;
}

void Window::set_mouse_up_callback(MouseCallback callback, void* userdata) {
	_mouse_up_cb = callback;
	_mouse_up_cb_userdata = userdata;
}

void Window::set_resize_callback(ResizeCallback callback, void* userdata) {
	_resize_cb = callback;
	_resize_cb_userdata = userdata;
}

} // namespace toy