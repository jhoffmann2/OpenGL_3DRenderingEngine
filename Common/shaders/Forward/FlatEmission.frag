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

in vec3 model_pos;
layout (location = 5) uniform sampler2D diffuseTexture;

out vec4 frag_color;

void main(void) {

	const vec2 uv = calculateUV(model_pos, vec2(0, 0), materials[curMaterial].textureMode_);
	const vec3 kd_tex = texture(diffuseTexture, uv).rgb;


	frag_color = vec4(materials[curMaterial].EmissiveColor * kd_tex, 1);
}
