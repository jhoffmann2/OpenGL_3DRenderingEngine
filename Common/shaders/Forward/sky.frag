/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SolidLighting.frag
Purpose: renders geometric solid using vertex lighting
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 11/03/2020
End Header --------------------------------------------------------*/ 
#version 460

in vec4 camRay;
out vec4 frag_color;

layout(location = 0) uniform sampler2D sky0;
layout(location = 1) uniform sampler2D sky1;
layout(location = 2) uniform sampler2D sky2;
layout(location = 3) uniform sampler2D sky3;
layout(location = 4) uniform sampler2D sky4;
layout(location = 5) uniform sampler2D sky5;
uniform sampler2D diffuseTexture;

vec2 cubicTextureMap(vec4 r, out uint cubeFace)
{
	r = normalize(r);
	if(abs(r.x) > abs(r.y) && abs(r.x) > abs(r.z))
	{
		cubeFace = (r.x > 0)? 0 : 2;
		r *= 1.f / abs(r.x);
		r /= 2;
		r += vec4(0.5, 0.5, 0.5, 0);
		return r.yz;
	}
	else if(abs(r.y) > abs(r.x) && abs(r.y) > abs(r.z))
	{
		cubeFace = (r.y > 0)? 4 : 5;
		r *= 1.f / abs(r.y);
		r /= 2;
		r += vec4(0.5, 0.5, 0.5, 0);
		return r.xz;
	}
	else if(abs(r.z) > abs(r.x) && abs(r.z) > abs(r.y))
	{
		cubeFace = (r.z > 0)? 1 : 3;
		r *= 1.f / abs(r.z);
		r /= 2;
		r += vec4(0.5, 0.5, 0.5, 0);
		return r.xy;
	}
}

vec4 getSkyColor()
{
	uint cubeFace;
	vec2 cubeUV = cubicTextureMap(normalize(camRay), cubeFace);
	switch(cubeFace)
	{
	case 0:
		return texture(sky0, cubeUV);
	case 1:
		return texture(sky1, cubeUV);
	case 2:
		return texture(sky2, cubeUV);
	case 3:
		return texture(sky3, cubeUV);
	case 4:
		return texture(sky4, cubeUV);
	case 5:
		return texture(sky5, cubeUV);
	}
}

void main(void) {
	frag_color = getSkyColor();
}