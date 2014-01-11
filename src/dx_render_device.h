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
		ES_POSITION, ES_NORMAL, ES_TEXCOORD, ES_COLOR
	};
	enum ElementFormat {
		EF_FLOAT4, EF_FLOAT3, EF_FLOAT2, EF_FLOAT1
	};

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
		MAX_INPUT_LAYOUTS = 4
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
private:
	bool create_back_buffer_and_dst();
	void setup_buffers();

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

	unsigned _n_constant_buffers;
	unsigned _n_vertex_buffers;
	unsigned _n_index_buffers;
};

} // namespace toy