#include "application.h"
#include "directx.h"
#include "math/float2.h"
#include "math/float3.h"
#include "math/float4.h"
#include <ctime>

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

} // anonymous namespace

bool Application::init(HINSTANCE instance) {
	_instance = instance;

	Window::register_class(instance);

	_main_window.open(instance, L"HLSL Toy", 1280, 720);
	if (!_render_device.init(_main_window)) {
		return false;
	}

	_toy_parameters_buffer = _render_device.create_constant_buffer(sizeof(ToyParameters));
	_vertices = _render_device.create_static_vertex_buffer(vertices, sizeof(vertices));
	_indices = _render_device.create_static_index_buffer(indices, sizeof(indices));

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