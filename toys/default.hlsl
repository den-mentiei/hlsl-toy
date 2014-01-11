struct PS_Input {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};


float4 ps_main(PS_Input input) : SV_TARGET0 {
	return float4(input.uv.x, input.uv.y, 0.0f, 1.0f);
}