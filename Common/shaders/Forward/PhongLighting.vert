/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongLighting.vert
Purpose: renders mesh with phong lighting
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 11/3/2020
End Header --------------------------------------------------------*/ 
#version 460
#include ../Include/lightingUniforms.glsl
#include ../Include/phong.glsl
#include ../Include/uv.glsl
#include ../Include/reflection.glsl
#include ../Include/refraction.glsl
#include ../Include/MeshProperties.glsl

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

out vec4 color;

void main() 
{
	vec4 world_normal = modelNToWorldN * normal;
	vec4 world_position = modelToWorld * position;
	gl_Position = camToNDC * worldToCam * world_position;

	const vec2 uv = calculateUV(position, uv_vert, materials[curMaterial].textureMode_);
	const vec3 kd_tex = texture(diffuseTexture, uv).rgb;
	const vec3 ks_tex = texture(specularTexture, uv).rgb;

	const vec4 N = normalize(world_normal);

	vec3 i_local = phongLight(world_position, N, kd_tex, ks_tex, curMaterial);

	const float Vlen = length(eyePos - world_position);
	float S = clamp(((fogFar - Vlen) / (fogFar - fogNear)), 0, 1);
	color = vec4((S * i_local) + ((1-S) * fogColor), 1);
}
