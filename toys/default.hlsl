// media/noise.png

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
Texture2D texture4;
Texture2D texture5;
Texture2D texture6;
Texture2D texture7;
Texture2D texture8;

SamplerState sampler_point : register(s0);
SamplerState sampler_linear : register(s1);
SamplerState sampler_aniso : register(s2);

struct PS_Input {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 ps_main(PS_Input input) : SV_TARGET0 {
	return float4(mouse.x / resolution.x, mouse.y / resolution.y, 0.0f, 1.0f);
}
