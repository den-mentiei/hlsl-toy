cbuffer Parameters {
	// mouse pixel coords. xy: current (if MLB down), zw: click
	float4 mouse;
	// current backbuffer size
	float2 resolution;
	// application time
	float time;
};

Texture2D texture0;
Texture2D texture1;
Texture2D texture2;
Texture2D texture3;

SamplerState sampler_point;
SamplerState sampler_linear;
SamplerState sampler_aniso;

struct PS_Input {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 ps_main(PS_Input input) : SV_TARGET0 {
	return texture0.Sample(sampler_linear, input.uv);
	return float4(mouse.x / resolution.x, mouse.y / resolution.y, 0.0f, 1.0f);
	return float4(input.uv.x * sin(time), input.uv.y * cos(time), 0.0f, 1.0f);
}