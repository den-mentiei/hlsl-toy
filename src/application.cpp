#include "application.h"

#include "directx.h"
#include "math/float2.h"
#include "math/float3.h"
#include "math/float4.h"

#include <ctime>
#include <cstring>

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

static const char* ps_shader_code = ""
"struct PS_Input {\n"
"	float4 pos : SV_POSITION;\n"
"	float2 uv : TEXCOORD;\n"
"};\n\n"
"float4 ps_main(PS_Input input) : SV_TARGET {\n"
"	return float4(input.uv.x, input.uv.y, 0.0f, 1.0f);\n"
"}";

} // anonymous namespace

bool Application::init(HINSTANCE instance) {
	_instance = instance;

	Window::register_class(instance);

	_main_window.open(instance, L"HLSL Toy", 1280, 720);
	if (!_render_device.init(_main_window)) {
		return false;
	}

	init_render();

	return true;
}

void Application::init_render() {
	_triangles.start_index = 0;
	_triangles.stride = sizeof(Vertex);
	_triangles.count = 6;

	_triangles.constants = _render_device.create_constant_buffer(sizeof(ToyParameters));

	_triangles.vertices = _render_device.create_static_vertex_buffer(vertices, sizeof(vertices));
	_triangles.indices = _render_device.create_static_index_buffer(indices, sizeof(indices));
	
	_triangles.vs = _render_device.create_vertex_shader(vs_shader_code, std::strlen(vs_shader_code), vertex_description);
	_triangles.ps = _render_device.create_pixel_shader(ps_shader_code, std::strlen(ps_shader_code));

	_triangles.dst_state = _render_device.create_dst_state(false);
	_triangles.rasterizer_state = _render_device.create_rasterizer_state();
	_triangles.blend_state = _render_device.create_blend_state(false);
}

void Application::shutdown() {
}

bool Application::work() {
	_toy_parameters.time = static_cast<float>(std::time(0));
	_render_device.update_constant_buffer(_triangles.constants, _toy_parameters);

	_render_device.set_viewport(_main_window.width(), _main_window.height());
	const Float4 clear_color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	_render_device.clear(clear_color);
	//_render_device.render(_triangles);
	_render_device.present();

	_main_window.update();

	return !_main_window.is_closing();
}

} // namespace toy