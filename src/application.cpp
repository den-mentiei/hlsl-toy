#include "application.h"
#include "directx.h"
#include <ctime>

namespace toy {

bool Application::init(HINSTANCE instance) {
	_instance = instance;

	Window::register_class(instance);

	_main_window.open(instance, L"HLSL Toy", 1280, 720);
	if (!_render_device.init(_main_window)) {
		return false;
	}

	_toy_parameters_buffer = _render_device.create_constant_buffer(sizeof(ToyParameters));
	return true;
}

void Application::shutdown() {
}

bool Application::work() {
	_toy_parameters.time = static_cast<float>(std::time(0));

	_render_device.update_constant_buffer(_toy_parameters_buffer, _toy_parameters);
	_main_window.update();

	return !_main_window.is_closing();
}

} // namespace toy