/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SolidShading.vert
Purpose: renders geometric solid using fragment shading
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 11/03/2020
End Header --------------------------------------------------------*/ 
#version 460
#include ../Include/MeshProperties.glsl

out vec3 light_pos;
out float light_radius;

void main() {
	light_pos = (modelToWorld * vec4(0,0,0,1)).xyz;
	vec4 world_position = (modelToWorld * vec4(position, 1));
	light_radius = length(world_position - vec4(light_pos, 1));
	gl_Position = camToNDC * worldToCam * world_position;
}
