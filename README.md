# hlsl-toy
----------
## Intro

HLSL shadertoy, inspired by Iñigo Quilez (www.shadertoy.com).

## Usage

1. Run.
	
	```
	./hlsl-toy.exe toys/default.hlsl
	```
	or to specify the resolution:

	```
	./hlsl-toy.exe toys/default.hlsl 1920 1080
	```

2. Edit your file in your favorite editor.
3. Press F5 to reload and check the results. Shader compilation errors will be reported if any.
4. Have fun!

## Advanced

You can import up to 8 textures. Just include textures file names in the shader source as comments. Textures will be reloaded automatically on F5 reload.

    // tex1.png
    // tex2.jpg
    // noise.bmp
    
    // ... shader code goes here ...

Set of parameters, that are exposed to the shader:

    cbuffer Parameters {
    	// mouse pixel coords. xy: current (if MLB down), zw: click
    	float4 mouse;
    	// current backbuffer size
    	float2 resolution;
    	// application global time
    	float time;
    };

There are predefined sampler states - all with wrap addressing:

	SamplerState sampler_point;
	SamplerState sampler_linear;
	SamplerState sampler_aniso;