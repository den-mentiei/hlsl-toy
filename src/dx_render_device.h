#pragma once

#include "directx.h"
#include "com_ptr.h"

namespace toy {

class Window;

class DXRenderDevice {
	static const unsigned MAX_CONSTANT_BUFFERS = 4;
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
	unsigned _n_constant_buffers;
};

} // namespace toy