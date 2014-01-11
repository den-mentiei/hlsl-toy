cbuffer Parameters {
	// mouse pixel coords. xy: current (if MLB down), zw: click
	float4 mouse;
	float2 resolution;
	float time;
};

struct PS_Input {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 ps_main(PS_Input input) : SV_TARGET0 {
	return float4(mouse.x / resolution.x, mouse.y / resolution.y, 0.0f, 1.0f);
	return float4(input.uv.x * sin(time), input.uv.y * cos(time), 0.0f, 1.0f);
}