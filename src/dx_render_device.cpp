#include "dx_render_device.h"

#include "window.h"

#include <cassert>

namespace toy {

// Order-dependent on VertexDescription enum
const unsigned VertexDescription::element_size[VertexDescription::EF_COUNT] = { sizeof(float) * 4, sizeof(float) * 3, sizeof(float) * 2, sizeof(float) };

DXRenderDevice::DXRenderDevice()
	: _n_constant_buffers(0)
	, _n_vertex_buffers(0)
	, _n_index_buffers(0)
	, _n_input_layouts(0)
	, _n_vertex_shaders(0)
	, _n_pixel_shaders(0)
	, _n_dst_states(0)
	, _n_rasterizer_states(0)
	, _n_blend_states(0)
{}

bool DXRenderDevice::init(const Window& window) {
	ComPtr<IDXGIFactory> dxgi_factory;
	HRESULT hr = ::CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgi_factory));
	if (FAILED(hr)) {
		return false;
	}

	ComPtr<IDXGIAdapter> adapter;
	// TODO: adapter index option
	hr = dxgi_factory->EnumAdapters(0, &adapter);
	if (FAILED(hr)) {
		return false;
	}

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
	hr = D3D11CreateDeviceAndSwapChain(
		adapter.get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		0,
#ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
#else
		0,
#endif
		feature_levels, n_feature_levels,
		D3D11_SDK_VERSION,
		&swapchain_description, &_swap_chain, &_device,
		&current_feature_level,
		&_immediate_device);
	if (FAILED(hr)) {
		return false;
	}

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

	return true;
}

unsigned DXRenderDevice::create_constant_buffer(const size_t size) {
	assert(_n_constant_buffers < MAX_CONSTANT_BUFFERS);

	D3D11_BUFFER_DESC buffer_description;
	buffer_description.Usage = D3D11_USAGE_DYNAMIC;
	buffer_description.ByteWidth = size;
	buffer_description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_description.MiscFlags = 0;
	buffer_description.StructureByteStride = 0;

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

unsigned DXRenderDevice::create_static_vertex_buffer(const void* const data, const size_t size) {
	assert(_n_vertex_buffers < MAX_VERTEX_BUFFERS);

	D3D11_BUFFER_DESC buffer_description;
	buffer_description.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_description.ByteWidth = size;
	buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_description.CPUAccessFlags = 0;
	buffer_description.MiscFlags = 0;
	buffer_description.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA resource;
	resource.SysMemPitch = 0;
	resource.SysMemSlicePitch = 0;
	resource.pSysMem = data;

	HRESULT hr = _device->CreateBuffer(&buffer_description, &resource, &_vertex_buffers[_n_vertex_buffers]);
	if (FAILED(hr)) {
		return MAX_VERTEX_BUFFERS + 1;
	}

	_n_vertex_buffers++;
	return _n_vertex_buffers - 1;
}

unsigned DXRenderDevice::create_static_index_buffer(const void* const data, const size_t size) {
	assert(_n_index_buffers < MAX_INDEX_BUFFERS);

	D3D11_BUFFER_DESC buffer_description;
	buffer_description.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_description.ByteWidth = size;
	buffer_description.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffer_description.CPUAccessFlags = 0;
	buffer_description.MiscFlags = 0;
	buffer_description.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA resource;
	resource.SysMemPitch = 0;
	resource.SysMemSlicePitch = 0;
	resource.pSysMem = data;

	HRESULT hr = _device->CreateBuffer(&buffer_description, &resource, &_index_buffers[_n_index_buffers]);
	if (FAILED(hr)) {
		return MAX_INDEX_BUFFERS + 1;
	}

	_n_index_buffers++;
	return _n_index_buffers - 1;
}

unsigned DXRenderDevice::create_input_layout(const VertexDescription& description, ComPtr<ID3D10Blob>& vs_blob) {
	assert(_n_input_layouts < MAX_INPUT_LAYOUTS);
	assert(description.n_elements > 0);

	// Order-dependent on VertexDescription enum
	static const char* semantics[] = {
		"POSITION", "NORMAL", "TEXCOORD", "COLOR"
	};
	// Order-dependent on VertexDescription enum, too
	static const DXGI_FORMAT formats[] = {
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32_FLOAT,
	};

	D3D11_INPUT_ELEMENT_DESC layout[VertexDescription::MAX_ELEMENTS];
	unsigned size = 0;
	for (unsigned i = 0; i < description.n_elements; ++i) {
		assert(description.elements[i].semantic < VertexDescription::ES_COUNT);
		assert(description.elements[i].semantic < VertexDescription::EF_COUNT);

		layout[i].SemanticName = semantics[description.elements[i].semantic];
		layout[i].SemanticIndex = 0;
		layout[i].Format = formats[description.elements[i].format];
		layout[i].InputSlot = 0;
		layout[i].AlignedByteOffset = size;
		layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout[i].InstanceDataStepRate = 0;

		size += VertexDescription::element_size[description.elements[i].format];
	}

	HRESULT hr = _device->CreateInputLayout(
		layout, description.n_elements,
		vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(),
		&_input_layouts[_n_input_layouts]);
	if (FAILED(hr)) {
		return MAX_INPUT_LAYOUTS + 1;
	}

	_n_input_layouts++;
	return _n_input_layouts - 1;
}

unsigned DXRenderDevice::create_vertex_shader(const char* const code, const size_t length, const VertexDescription& vertex_description) {
	assert(code != nullptr);
	assert(length > 0);

	ComPtr<ID3DBlob> vs_blob;
	HRESULT hr = D3DX11CompileFromMemory(code, length, 0, 0, 0, "vs_main", "vs_4_0", 0, 0, 0, &vs_blob, 0, 0);
	if (FAILED(hr)) {
		return MAX_VERTEX_SHADERS + 1;
	}

	hr = _device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &_vertex_shaders[_n_vertex_shaders]);
	if (FAILED(hr)) {
		return MAX_VERTEX_SHADERS + 1;
	}

	_vertex_shader_il[_n_vertex_shaders] = create_input_layout(vertex_description, vs_blob);

	_n_vertex_shaders++;
	return _n_vertex_shaders - 1;
}

unsigned DXRenderDevice::create_pixel_shader(const char* const code, const size_t length) {
	assert(code != nullptr);
	assert(length > 0);

	if (!compile_pixel_shader(code, length, _pixel_shaders[_n_pixel_shaders])) {
		return MAX_PIXEL_SHADERS + 1;
	}

	_n_pixel_shaders++;
	return _n_pixel_shaders - 1;
}

void DXRenderDevice::update_pixel_shader(unsigned shader, const char* const code, const size_t length) {
	assert(shader < _n_pixel_shaders);
	assert(code != nullptr);
	assert(length > 0);

	if (!compile_pixel_shader(code, length, _pixel_shaders[shader])) {
		// TODO: report errors somehow
	}
}

bool DXRenderDevice::compile_pixel_shader(const char* const code, const size_t length, ComPtr<ID3D11PixelShader>& destination) {
	ComPtr<ID3DBlob> ps_blob;
	HRESULT hr = D3DX11CompileFromMemory(code, length, 0, 0, 0, "ps_main", "ps_4_0", 0, 0, 0, &ps_blob, 0, 0);
	if (FAILED(hr)) {
		return false;
	}

	ID3D11PixelShader* ps = nullptr;
	hr = _device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &ps);
	if (FAILED(hr)) {
		return false;
	}
	destination.set(ps);

	return true;
}

unsigned DXRenderDevice::create_dst_state(const bool depth_enabled) {
	assert(_n_dst_states < MAX_DST_STATES);

	CD3D11_DEPTH_STENCIL_DESC dst_description(D3D11_DEFAULT);
	dst_description.DepthEnable = depth_enabled;

	HRESULT hr = _device->CreateDepthStencilState(&dst_description, &_dst_states[_n_dst_states]);
	if (FAILED(hr)) {
		return MAX_DST_STATES + 1;
	}

	_n_dst_states++;
	return _n_dst_states - 1;
}

unsigned DXRenderDevice::create_rasterizer_state() {
	assert(_n_rasterizer_states < MAX_RASTERIZER_STATES);

	CD3D11_RASTERIZER_DESC rs_description(D3D11_DEFAULT);
	// rs_description.CullMode = D3D11_CULL_NONE;
	rs_description.DepthClipEnable = false;
	
	HRESULT hr = _device->CreateRasterizerState(&rs_description, &_rasterizer_states[_n_rasterizer_states]);
	if (FAILED(hr)) {
		return MAX_RASTERIZER_STATES + 1;
	}

	_n_rasterizer_states++;
	return _n_rasterizer_states - 1;
}

unsigned DXRenderDevice::create_blend_state(const bool blend_enabled) {
	assert(_n_blend_states < MAX_BLEND_STATES);

	D3D11_RENDER_TARGET_BLEND_DESC rt_bs_description;
	rt_bs_description.BlendEnable = blend_enabled;
	rt_bs_description.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rt_bs_description.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rt_bs_description.BlendOp = D3D11_BLEND_OP_ADD;
	rt_bs_description.SrcBlendAlpha = D3D11_BLEND_ONE;
	rt_bs_description.DestBlendAlpha = D3D11_BLEND_ZERO;
	rt_bs_description.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rt_bs_description.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC bs_description;
	bs_description.AlphaToCoverageEnable = false;
	bs_description.IndependentBlendEnable = false;
	bs_description.RenderTarget[0] = rt_bs_description;

	HRESULT hr = _device->CreateBlendState(&bs_description, &_blend_states[_n_blend_states]);
	if (FAILED(hr)) {
		return MAX_BLEND_STATES + 1;
	}

	_n_blend_states++;
	return _n_blend_states - 1;
}

void DXRenderDevice::render(const Batch& batch) {
	UINT stride = batch.stride;
	UINT offset = 0;

	ID3D11Buffer* vertex_buffers[] = { _vertex_buffers[batch.vertices].get() };
	_immediate_device->IASetVertexBuffers(0, 1, vertex_buffers, &stride, &offset);
	_immediate_device->IASetIndexBuffer(_index_buffers[batch.indices].get(), DXGI_FORMAT_R32_UINT, 0);
	_immediate_device->IASetInputLayout(_input_layouts[_vertex_shader_il[batch.vs]].get());
	_immediate_device->IASetPrimitiveTopology(
		batch.type == Batch::BT_TRIANGLE_LIST ? 
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	ID3D11Buffer* constant_buffers[] = { _constant_buffers[batch.constants].get() };
	_immediate_device->PSSetConstantBuffers(0, 1, constant_buffers);

	_immediate_device->OMSetDepthStencilState(_dst_states[batch.dst_state].get(), 0);
	_immediate_device->OMSetBlendState(_blend_states[batch.blend_state].get(), 0, 0xFFFFFFFF);
	_immediate_device->RSSetState(_rasterizer_states[batch.rasterizer_state].get());

	_immediate_device->VSSetShader(_vertex_shaders[batch.vs].get(), 0, 0);
	_immediate_device->GSSetShader(0, 0, 0);
	_immediate_device->PSSetShader(_pixel_shaders[batch.ps].get(), 0, 0);
	_immediate_device->DrawIndexed(batch.count, batch.start_index, 0);
}

void DXRenderDevice::clear(const Float4 clear_color) {
	_immediate_device->ClearRenderTargetView(_back_buffer_rtv.get(), &clear_color.x);
	_immediate_device->ClearDepthStencilView(_depth_stencil_view.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 255);
}

void DXRenderDevice::set_viewport(const unsigned w, const unsigned h) {
	D3D11_VIEWPORT viewport;

	viewport.Width = float(w);
	viewport.Height = float(h);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	_immediate_device->RSSetViewports(1, &viewport);
}

void DXRenderDevice::start_frame() {
	ID3D11RenderTargetView* rtvs[] = { _back_buffer_rtv.get() };
	_immediate_device->OMSetRenderTargets(1, rtvs, _depth_stencil_view.get());
}

void DXRenderDevice::end_frame() {
	_swap_chain->Present(0, 0);
}

} // namespace toy
