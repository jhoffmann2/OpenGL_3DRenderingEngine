/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongShading.frag
Purpose: renders mesh with phong shading
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 10/26/2020
End Header --------------------------------------------------------*/ 
#version 460
#include ../Include/lightingUniforms.glsl
#include ../Include/uv.glsl
#include ../Include/phong.glsl

vec3 shading_method(vec3 P, vec3 N, vec3 kd_tex, vec3 ks_tex, uint materialIndex)
{
	return phongLight(P,N,kd_tex,ks_tex,materialIndex);
}
#include ../Include/general_shading.frag