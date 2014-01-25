#include "application.h"

#include "options.h"
#include "directx.h"
#include "math/float2.h"
#include "math/float3.h"
#include "math/float4.h"

#include <cmath>
#include <ctime>
#include <cstring>
#include <algorithm>

namespace toy {

namespace {

struct Vertex {
	Float3 pos;
	Float2 uv;
};

static const Vertex vertices[] = {
	{ float3(-1.0f, -1.0f, 0.0f), float2(0.0f, 1.0f) },
	{ float3(-1.0f,  1.0f, 0.0f), float2(0.0f, 0.0f) },
	{ float3( 1.0f,  1.0f, 0.0f), float2(1.0f, 0.0f) },
	{ float3( 1.0f, -1.0f, 0.0f), float2(1.0f, 1.0f) },
};

static const unsigned indices[] = {
	0, 1, 2,
	0, 2, 3
};

static const VertexDescription vertex_description = {
	// elements
	{
		{ VertexDescription::ES_POSITION, VertexDescription::EF_FLOAT3 },
		{ VertexDescription::ES_TEXCOORD, VertexDescription::EF_FLOAT2 },
	},
	// n_elements
	2
};

// Where is my raw string literals, Microsoft?
static const char* vs_shader_code = ""
"struct VS_Input {\n"
"	float3 pos : POSITION;\n"
"	float2 uv : TEXCOORD;\n"
"};\n\n"
"struct PS_Input {\n"
"	float4 pos : SV_POSITION;\n"
"	float2 uv : TEXCOORD;\n"
"};\n\n"
"PS_Input vs_main(VS_Input input) {\n"
"	PS_Input o;\n"
"	o.pos = float4(input.pos.x, input.pos.y, 0.0f, 1.0f);\n"
"	o.uv = input.uv;\n"
"	return o;\n"
"}";

} // anonymous namespace

bool Application::init(HINSTANCE instance, const Options& options) {
	_instance = instance;

	Window::register_class(instance);

	_main_window.open(instance, L"HLSL Toy v1.0 (c) Denis Mentey", options.w(), options.h());
	_main_window.set_keypress_callback(Application::on_keypress_callback, this);
	_main_window.set_mouse_move_callback(Application::on_mouse_move_callback, this);
	if (!_render_device.init(_main_window)) {
		return false;
	}

	_main_window.set_resize_callback(Application::on_resize_callback, this);

	if (options.toy_file())
		_toy_filename = options.toy_file();
	else
		_toy_filename = std::move(_main_window.open_file_dialog(L"HLSL (*.hlsl)\0*.hlsl\0All (*.*)\0*.*\0"));

	if (_toy_filename.empty()) {
		return false;
	}

	const auto last_separator = _toy_filename.find_last_of('\\') + 1;
	_toy_path = std::wstring(_toy_filename.cbegin(), _toy_filename.cbegin() + last_separator);

	if (!load_toy(_toy_filename.c_str())) {
		return false;
	}

	create_scene();
	_toy_parameters.mouse = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return true;
}

bool Application::load_toy(const wchar_t* path) {
	if (!_toy.init(path)) {
		return false;
	}
	return true;
}

void Application::create_scene() {
	_triangles.start_index = 0;
	_triangles.stride = sizeof(Vertex);
	_triangles.count = 6;
	_triangles.type = DXRenderDevice::Batch::BT_TRIANGLE_LIST;

	_triangles.constants = _render_device.create_constant_buffer(sizeof(ToyParameters));

	_triangles.vertices = _render_device.create_static_vertex_buffer(vertices, sizeof(vertices));
	_triangles.indices = _render_device.create_static_index_buffer(indices, sizeof(indices));
	
	_triangles.vs = _render_device.create_vertex_shader(vs_shader_code, std::strlen(vs_shader_code), vertex_description);
	_triangles.ps = _render_device.create_pixel_shader(_toy.code(), _toy.code_length());

	_triangles.dst_state = _render_device.create_dst_state(false);
	_triangles.rasterizer_state = _render_device.create_rasterizer_state();
	_triangles.blend_state = _render_device.create_blend_state(false);

	const unsigned n_textures = std::max(0u, std::min(_toy.n_textures(), static_cast<unsigned>(DXRenderDevice::MAX_TEXTURES)));
	for (unsigned i = 0; i < n_textures; ++i) {
		std::wstring fullname = _toy_path + _toy.texture_path(i);
		_triangles.textures[i] = _render_device.create_texture(fullname.c_str());
	}
	_triangles.n_textures = n_textures;

	_triangles.samplers[0] = _render_device.create_sampler(DXRenderDevice::SF_POINT, DXRenderDevice::SA_WRAP);
	_triangles.samplers[1] = _render_device.create_sampler(DXRenderDevice::SF_LINEAR, DXRenderDevice::SA_WRAP);
	_triangles.samplers[2] = _render_device.create_sampler(DXRenderDevice::SF_ANISO, DXRenderDevice::SA_WRAP);
	_triangles.n_samplers = 3;
}

bool Application::work() {
	update();
	render();

	return !_main_window.is_closing();
}

void Application::update() {
	_timer.tick();
	_main_window.update();
	update_toy_parameters();
}

void Application::render() {
	_render_device.start_frame();
	_render_device.set_viewport(_main_window.width(), _main_window.height());
	const Float4 clear_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	_render_device.clear(clear_color);
	_render_device.render(_triangles);
	_render_device.end_frame();
}

void Application::update_toy_parameters() {
	_toy_parameters.resolution = float2(float(_main_window.width()), float(_main_window.height()));
	_toy_parameters.time = static_cast<float>(_timer.elapsed());

	_render_device.update_constant_buffer(_triangles.constants, _toy_parameters);
}

void Application::on_resize_callback(const unsigned w, const unsigned h, void* userdata) {
	Application* app = static_cast<Application*>(userdata);
	app->handle_resize(w, h);
}

void Application::handle_resize(const unsigned w, const unsigned h) {
	_render_device.resize_swapchain(w, h);
}

void Application::on_keypress_callback(const unsigned key_code, void* userdata) {
	Application* app = static_cast<Application*>(userdata);
	app->handle_keypress(key_code);
}

void Application::handle_keypress(const unsigned key_code) {
	switch (key_code) {
		case VK_F5:
			reload();
			break;
	}
}

void Application::on_mouse_move_callback(const unsigned x, const unsigned y, Mouse::Button button, void* userdata) {
	Application* app = static_cast<Application*>(userdata);
	app->handle_mouse_move(x, y, button);
}

void Application::handle_mouse_move(const unsigned x, const unsigned y, Mouse::Button button) {
	if (button == Mouse::B_LEFT) {
		_toy_parameters.mouse.x = float(x);
		_toy_parameters.mouse.y = float(y);
	}
}

void Application::on_mouse_down_callback(const unsigned x, const unsigned y, Mouse::Button button, void* userdata) {
	Application* app = static_cast<Application*>(userdata);
	app->handle_mouse_down(x, y, button);
}

void Application::handle_mouse_down(const unsigned x, const unsigned y, Mouse::Button button) {
	if (button == Mouse::B_LEFT) {
		_toy_parameters.mouse.z = float(x);
		_toy_parameters.mouse.w = float(y);
	}
}

void Application::on_mouse_up_callback(const unsigned x, const unsigned y, Mouse::Button button, void* userdata) {
	Application* app = static_cast<Application*>(userdata);
	app->handle_mouse_up(x, y, button);
}

void Application::handle_mouse_up(const unsigned x, const unsigned y, Mouse::Button button) {
	if (button == Mouse::B_LEFT) {
		_toy_parameters.mouse.z = -std::abs(_toy_parameters.mouse.z);
		_toy_parameters.mouse.w = -std::abs(_toy_parameters.mouse.w);
	}
}

void Application::reload() {
	if (!_toy.init(_toy_path.c_str())) {
		// TODO: report errors
		return;
	}
	_render_device.update_pixel_shader(_triangles.ps, _toy.code(), _toy.code_length());

	const unsigned n_toy_textures = _toy.n_textures();
	const unsigned n_updated_textures = std::min(n_toy_textures, _triangles.n_textures);

	// Updates existing textures
	for (unsigned i = 0; i < n_updated_textures; ++i) {
		_render_device.update_texture(i, _toy.texture_path(i));
	}
	// Appends new ones
	for (unsigned i = n_updated_textures; i < _toy.n_textures(); ++i) {
		_triangles.textures[i] = _render_device.create_texture(_toy.texture_path(i));
	}
	_triangles.n_textures = _toy.n_textures();
}

} // namespace toy