#pragma once

#include "directx.h"
#include "math/types.h"
#include "com_ptr.h"

namespace toy {

class Window;

struct VertexDescription {
	enum {
		MAX_ELEMENTS = 8
	};

	enum ElementSemantic {
		ES_POSITION = 0, ES_NORMAL, ES_TEXCOORD, ES_COLOR, 
		ES_COUNT
	};
	enum ElementFormat {
		EF_FLOAT4 = 0, EF_FLOAT3, EF_FLOAT2, EF_FLOAT1, 
		EF_COUNT
	};
	static const unsigned element_size[EF_COUNT];

	struct Element {
		ElementSemantic semantic;
		ElementFormat format;
	};
	Element elements[MAX_ELEMENTS];
	unsigned n_elements;
};

class DXRenderDevice {
public:
	enum {
		MAX_CONSTANT_BUFFERS = 4,
		MAX_VERTEX_BUFFERS = 4,
		MAX_INDEX_BUFFERS = 4,
		MAX_INPUT_LAYOUTS = 4,
		MAX_VERTEX_SHADERS = 4,
		MAX_PIXEL_SHADERS = 4,
		MAX_DST_STATES = 4,
		MAX_RASTERIZER_STATES = 4,
		MAX_BLEND_STATES = 4,
		MAX_TEXTURES = 8,
		MAX_SAMPLERS = 16
	};

	DXRenderDevice();

	bool init(const Window& window);

	unsigned create_constant_buffer(const size_t size);
	void update_constant_buffer(const unsigned id, const void* const data, const size_t size);
	template <typename T>
	void update_constant_buffer(const unsigned id, const T& data) {
		update_constant_buffer(id, &data, sizeof(T));
	}

	unsigned create_static_vertex_buffer(const void* const data, const size_t size);
	unsigned create_static_index_buffer(const void* const data, const size_t size);

	unsigned create_vertex_shader(const char* const code, const size_t length, const VertexDescription& vertex_description);
	unsigned create_pixel_shader(const char* const code, const size_t length);

	void update_pixel_shader(const unsigned shader, const char* const code, const size_t length);

	unsigned create_dst_state(const bool depth_enabled);
	unsigned create_rasterizer_state();
	unsigned create_blend_state(const bool blend_enabled);

	unsigned create_texture(const wchar_t* const path);
	void update_texture(const unsigned texture, const wchar_t* const path);

	enum SamplerFilter {
		SF_POINT, SF_LINEAR, SF_ANISO
	};
	enum SamplerAddress {
		SA_WRAP, SA_CLAMP
	};
	unsigned create_sampler(SamplerFilter filter, SamplerAddress address);

	struct Batch {
		enum Type {
			BT_TRIANGLE_LIST,
			BT_TRIANGLE_STRIP
		};

		enum {
			B_MAX_TEXTURES = 4,
			B_MAX_SAMPLERS = 4
		};

		size_t start_index;
		size_t count;
		size_t stride;
		unsigned vertices;
		unsigned indices;
		unsigned constants;
		unsigned dst_state;
		unsigned rasterizer_state;
		unsigned blend_state;
		unsigned vs;
		unsigned ps;
		unsigned textures[B_MAX_TEXTURES];
		unsigned n_textures;
		unsigned samplers[B_MAX_SAMPLERS];
		unsigned n_samplers;
		Type type;
	};

	void resize_swapchain(const unsigned w, const unsigned h);
	void clear(const Float4 clear_color);
	void set_viewport(const unsigned w, const unsigned h);
	void render(const Batch& batch);
	void start_frame();
	void end_frame();
private:
	bool create_back_buffer_and_dst();
	void setup_buffers();

	unsigned create_input_layout(const VertexDescription& description, ComPtr<ID3D10Blob>& vs_blob);

	bool compile_pixel_shader(const char* const code, const size_t length, ComPtr<ID3D11PixelShader>& destination);
	bool load_texture(const wchar_t* const path, ComPtr<ID3D11ShaderResourceView>& destination);

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _immediate_device;
	ComPtr<IDXGISwapChain> _swap_chain;

	ComPtr<ID3D11Texture2D> _back_buffer;
	ComPtr<ID3D11Texture2D> _depth_stencil;
	ComPtr<ID3D11RenderTargetView> _back_buffer_rtv;
	ComPtr<ID3D11DepthStencilView> _depth_stencil_view;

	ComPtr<ID3D11Buffer> _constant_buffers[MAX_CONSTANT_BUFFERS];
	ComPtr<ID3D11Buffer> _vertex_buffers[MAX_VERTEX_BUFFERS];
	ComPtr<ID3D11Buffer> _index_buffers[MAX_INDEX_BUFFERS];
	ComPtr<ID3D11InputLayout> _input_layouts[MAX_INPUT_LAYOUTS];
	ComPtr<ID3D11VertexShader> _vertex_shaders[MAX_VERTEX_SHADERS];
	ComPtr<ID3D11PixelShader> _pixel_shaders[MAX_PIXEL_SHADERS];
	ComPtr<ID3D11DepthStencilState> _dst_states[MAX_DST_STATES];
	ComPtr<ID3D11RasterizerState> _rasterizer_states[MAX_DST_STATES];
	ComPtr<ID3D11BlendState> _blend_states[MAX_BLEND_STATES];
	ComPtr<ID3D11ShaderResourceView> _texture_srvs[MAX_TEXTURES];
	ComPtr<ID3D11SamplerState> _sampler_states[MAX_SAMPLERS];

	unsigned _vertex_shader_il[MAX_VERTEX_SHADERS];

	unsigned _n_constant_buffers;
	unsigned _n_vertex_buffers;
	unsigned _n_index_buffers;
	unsigned _n_input_layouts;
	unsigned _n_vertex_shaders;
	unsigned _n_pixel_shaders;
	unsigned _n_dst_states;
	unsigned _n_rasterizer_states;
	unsigned _n_blend_states;
	unsigned _n_texture_srvs;
	unsigned _n_sampler_states;
};

} // namespace toy