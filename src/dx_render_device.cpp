#include "dx_render_device.h"
#include "window.h"

namespace toy {
	
bool DXRenderDevice::init(const Window& window) {
	IDXGIFactory* dxgi_factory = nullptr;
	HRESULT hr = ::CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgi_factory));
	if (FAILED(hr)) {
		return false;
	}

	IDXGIAdapter* adapter = nullptr;
	// TODO: adapter index option
	hr = dxgi_factory->EnumAdapters(0, &adapter);
	if (FAILED(hr)) {
		return false;
	}
	dxgi_factory->Release();

	DXGI_SWAP_CHAIN_DESC swapchain_description;
	memset(&swapchain_description, 0, sizeof(swapchain_description));
	swapchain_description.BufferDesc.Width = 0;
	swapchain_description.BufferDesc.Height = 0;
	// TODO: sRGB option
	swapchain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_description.BufferDesc.RefreshRate.Numerator = 60;
	swapchain_description.BufferDesc.RefreshRate.Denominator = 1;
	// TODO: MSAA options
	swapchain_description.SampleDesc.Count = 1;
	swapchain_description.SampleDesc.Quality = 0;
	swapchain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_description.BufferCount = 2;
	swapchain_description.OutputWindow = window.handle();
	swapchain_description.Windowed = true;
	swapchain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	const D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	const unsigned n_feature_levels = sizeof(feature_levels) / sizeof(feature_levels[0]);

	D3D_FEATURE_LEVEL current_feature_level;
	hr = D3D11CreateDeviceAndSwapChain(adapter, 
		D3D_DRIVER_TYPE_UNKNOWN, 
		0, 0, 
		feature_levels, n_feature_levels, 
		D3D11_SDK_VERSION,
		&swapchain_description, &_swap_chain, &_device,
		&current_feature_level,
		&_immediate_device);
	if (FAILED(hr)) {
		return false;
	}

	adapter->Release();

	return true;
}

} // namespace toy