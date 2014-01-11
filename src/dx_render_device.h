#pragma once

#include "directx.h"
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
	enum {
		MAX_CONSTANT_BUFFERS = 4,
		MAX_VERTEX_BUFFERS = 4,
		MAX_INDEX_BUFFERS = 4,
		MAX_INPUT_LAYOUTS = 4,
		MAX_VERTEX_SHADERS = 4,
		MAX_PIXEL_SHADERS = 4,
		MAX_DST_STATES = 4,
		MAX_RASTERIZER_STATES = 4,
		MAX_BLEND_STATES = 4
	};
public:
	DXRenderDevice();

	bool init(const Window& window);
	void shutdown();

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

	unsigned create_dst_state(const bool depth_enabled, const bool stencil_enabled);
	unsigned create_rasterizer_state();
	unsigned create_blend_state();
private:
	bool create_back_buffer_and_dst();
	void setup_buffers();

	unsigned create_input_layout(const VertexDescription& description, ComPtr<ID3D10Blob>& vs_blob);

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
	ComPtr<ID3D11InputLayout> _input_layout[MAX_INPUT_LAYOUTS];
	ComPtr<ID3D11VertexShader> _vertex_shaders[MAX_VERTEX_SHADERS];
	ComPtr<ID3D11PixelShader> _pixel_shaders[MAX_PIXEL_SHADERS];
	ComPtr<ID3D11DepthStencilState> _dst_states[MAX_DST_STATES];
	ComPtr<ID3D11RasterizerState> _rasterizer_states[MAX_DST_STATES];
	ComPtr<ID3D11BlendState> _blend_states[MAX_BLEND_STATES];

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
};

} // namespace toy