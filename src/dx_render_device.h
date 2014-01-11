#pragma once

#include "directx.h"
#include "com_ptr.h"

namespace toy {

class Window;

class DXRenderDevice {
public:
	bool init(const Window& window);
	void shutdown();
private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _immediate_device;
	ComPtr<IDXGISwapChain> _swap_chain;
};

} // namespace toy