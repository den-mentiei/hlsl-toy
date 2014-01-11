cbuffer Parameters {
	float2 resolution;
	float time;
};

struct PS_Input {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 ps_main(PS_Input input) : SV_TARGET0 {
	return float4(input.uv.x * sin(time), input.uv.y * cos(time), 0.0f, 1.0f);
}