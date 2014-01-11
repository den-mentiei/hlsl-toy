#include "dx_render_device.h"
#include "window.h"
#include <cassert>

namespace toy {

DXRenderDevice::DXRenderDevice()
	: _n_constant_buffers(0)
{}

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

	return create_back_buffer_and_dst();
}

bool DXRenderDevice::create_back_buffer_and_dst() {
	HRESULT hr = _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_back_buffer));
	if (FAILED(hr)) {
		return false;
	}

	D3D11_TEXTURE2D_DESC back_buffer_description;
	_back_buffer->GetDesc(&back_buffer_description);

	D3D11_RENDER_TARGET_VIEW_DESC rtv_description;
	// TODO: MSAA support
	rtv_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_description.Texture2DArray.MipSlice = 0;
	rtv_description.Texture2DArray.ArraySize = 1;
	rtv_description.Format = back_buffer_description.Format;

	hr = _device->CreateRenderTargetView(_back_buffer.get(), &rtv_description, &_back_buffer_rtv);
	if (FAILED(hr)) {
		return false;
	}

	D3D11_TEXTURE2D_DESC dst_description;
	dst_description.Width = back_buffer_description.Width;
	dst_description.Height = back_buffer_description.Height;
	dst_description.MipLevels = 1;
	dst_description.ArraySize = 1;
	dst_description.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dst_description.SampleDesc.Count = 1;
	dst_description.SampleDesc.Quality = 0;
	dst_description.Usage = D3D11_USAGE_DEFAULT;
	dst_description.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dst_description.CPUAccessFlags = 0;
	dst_description.MiscFlags = 0;

	hr = _device->CreateTexture2D(&dst_description, 0, &_depth_stencil);
	if (FAILED(hr)) {
		return false;
	}

	hr = _device->CreateDepthStencilView(_depth_stencil.get(), 0, &_depth_stencil_view);
	if (FAILED(hr)) {
		return false;
	}

	_immediate_device->OMSetRenderTargets(1, &_back_buffer_rtv, _depth_stencil_view.get());

	return true;
}

unsigned DXRenderDevice::create_constant_buffer(const size_t size) {
	assert(_n_constant_buffers < MAX_CONSTANT_BUFFERS);

	D3D11_BUFFER_DESC buffer_description;
	memset(&buffer_description, 0, sizeof(D3D11_BUFFER_DESC));
	buffer_description.Usage = D3D11_USAGE_DYNAMIC;
	buffer_description.ByteWidth = size;
	buffer_description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_description.MiscFlags = 0;

	HRESULT hr = _device->CreateBuffer(&buffer_description, 0, &_constant_buffers[_n_constant_buffers]);
	if (FAILED(hr)) {
		return MAX_CONSTANT_BUFFERS + 1;
	}
	_n_constant_buffers++;

	return _n_constant_buffers - 1;
}

void DXRenderDevice::update_constant_buffer(const unsigned id, const void* const data, const size_t size) {
	assert(id < _n_constant_buffers);
	assert(data != nullptr);
	assert(size > 0);

	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = _immediate_device->Map(_constant_buffers[id].get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(hr)) {
		return;
	}

	memcpy(resource.pData, data, size);
	_immediate_device->Unmap(_constant_buffers[id].get(), 0);
}

} // namespace toy