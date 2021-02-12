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
#include ../Include/lightingUniforms.glsl
#include ../Include/phong.glsl

uniform sampler2D diffuseTex;  // = { kd.r, kd.g, kd.b, ks }
uniform sampler2D worldPosTex; // = { P.x, P.y, P.z, materialIndex }
uniform sampler2D normalTex;   // = { N.x, N.y, N.z, 0 }

in vec2 uv_frag;
out vec4 frag_color;

void main(void)
{
	vec4 P = texture(worldPosTex, uv_frag);
	const float Vlen = length(eyePos.xyz - P.xyz);
	float S = clamp(((fogFar - Vlen) / (fogFar - fogNear)), 0, 1);
	if(S == 0) // break out early to avoid unneccesary calculations on pixels that are shrouded in fog
	{
		frag_color = vec4(fogColor , 1);
		return;
	}
	
	vec4 N = texture(normalTex, uv_frag);

	uint material = uint(P.w * 10);
	P.w = 1;

	vec4 kd = texture(diffuseTex, uv_frag);
	vec3 ks = vec3(kd.w);

	vec3 i_local = (S < 1)? phongLight(P.xyz, N.xyz, kd.rgb, ks, material) : vec3(0);
		

	frag_color = vec4(mix(fogColor, i_local, S), 1);
}
