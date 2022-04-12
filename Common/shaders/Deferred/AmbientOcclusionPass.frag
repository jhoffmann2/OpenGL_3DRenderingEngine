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
#include ../Include/ShaderGlobals.glsl
#include ../Include/lightingUniforms.glsl

#define PI 3.14159265359f

layout (location = 1) uniform sampler2D diffuseTex;  // = { kd.r, kd.g, kd.b, ks }
layout (location = 2) uniform sampler2D worldPosTex; // = { P.x, P.y, P.z, materialIndex }
layout (location = 3) uniform sampler2D normalTex;   // = { N.x, N.y, N.z, 0 }

layout (location = 6) uniform uint width;
layout (location = 7) uniform uint height;

in vec2 uv_frag;
layout (location = 4) out vec4 ambientOcclusionOut;

const int n = 20;
const float delta = 0.001f;
const float c = 0.1f * aoRadius;

void main(void)
{
	vec4 N = texture(normalTex, uv_frag);
	if(abs(N.x) > 1000.f) // ignore fragments without geometry
	{
		ambientOcclusionOut = vec4(1);
		return;
	}

	vec4 P = texture(worldPosTex, uv_frag);
	uint material = uint(P.w * 10);
	P.w = 1;

	vec4 kd = texture(diffuseTex, uv_frag);
	vec3 ks = vec3(kd.w);
	kd.w = 1;


	const ivec2 fc = ivec2(int(gl_FragCoord.x), int(gl_FragCoord.y));
	const vec2 nfc = vec2(gl_FragCoord.x / width, gl_FragCoord.y / height);
	const int phi = ((30 * fc.x) ^ fc.y) + (10 * fc.x * fc.y) % 31416;
	const vec4 camPos = worldToCam * P;
	const float d = -(camPos).z;

	// generate pseudo-random uv coords in a spiral
	vec2 rand[n];
	for(int i = 0; i < n; ++i)
	{
		const float alpha = (i + 0.5f) / n;

		const float h = alpha * aoRadius / d;
		const float theta = 2.f * PI * alpha * (7.f * n / 9.f) + phi;
		rand[i] = clamp(nfc + h * vec2(cos(theta), sin(theta)), vec2(0,0), vec2(1,1));
	}

	// calculate ambient occlusion factor
	float S = 0;
	for(int i = 0; i < n; ++i)
	{
		vec4 P2 = texture(worldPosTex, rand[i]);
		P2.w = 1.f;
		if(abs(P2.x) > 1000.f) // ignore fragments without geometry
		{
			continue;
		}


		const vec4 camPos2 = worldToCam * P2;
		const float d2 = -(camPos2).z;

		vec4 omega = P2 - P;
		const float H = ((aoRadius - length(omega)) < 0) ? 0 : 1;

		S += (max(0.f, dot(N, omega) - delta * d2) * H) / max(c * c, dot(omega, omega));
	}
	S *= (2 * PI * c) / n;

	ambientOcclusionOut = vec4(max(0, pow(1 - aoIntensity * S, aoContrast)));

}
