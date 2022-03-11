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
#include ../Include/uv.glsl

in vec3 world_position;
in vec3 world_normal;

in vec3 model_pos;
in vec2 uv_frag;
in float local_mat;

layout (location = 5) uniform sampler2D diffuseTexture;
layout (location = 6) uniform sampler2D specularTexture;

layout (location = 0) out vec4 DiffuseOut;
layout (location = 1) out vec4 WorldPosOut;
layout (location = 2) out vec4 NormalOut;


void main(void)
{
	const vec2 uv = calculateUV(model_pos, uv_frag, materials[curMaterial].textureMode_);
	DiffuseOut.rgb = texture(diffuseTexture, uv).rgb;
	DiffuseOut.a = texture(specularTexture, uv).r;

	WorldPosOut.xyz = world_position;
	
	WorldPosOut.w = 0.1f * (
		abs(local_mat + 1.f) < 0.05f ? // local_mat ~== -1.f
			float(curMaterial) : 
			local_mat
	);

	NormalOut.xyz = normalize(world_normal);
}
