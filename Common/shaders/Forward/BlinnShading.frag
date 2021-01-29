/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongShading.frag
Purpose: renders mesh with Blinn Shading
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 11/03/2020
End Header --------------------------------------------------------*/ 
#version 460
#include ../Include/lightingUniforms.glsl
#include ../Include/blinn.glsl
#include ../Include/uv.glsl
#include ../Include/reflection.glsl
#include ../Include/refraction.glsl

in vec4 model_pos;
in vec2 uv_frag;

in vec4 world_position;
in vec4 world_normal;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

out vec4 frag_color;

void main(void) 
{
	const vec2 uv = calculateUV(model_pos, uv_frag, materials[curMaterial].textureMode_);
	const vec3 kd_tex = texture(diffuseTexture, uv).rgb;
	const vec3 ks_tex = texture(specularTexture, uv).rgb;

	const vec4 N = normalize(world_normal);
	
	vec3 i_local = blinnLight(world_position, N, kd_tex, ks_tex, curMaterial);

	const float Vlen = length(eyePos - world_position);
	float S = clamp(((fogFar - Vlen) / (fogFar - fogNear)), 0, 1);

	frag_color = vec4(mix(fogColor, i_local, S), 1);
}