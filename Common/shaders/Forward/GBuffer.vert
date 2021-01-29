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

in vec4 position;
in vec4 normal;
in vec2 uv_vert;

layout (std140, binding = 2) uniform VertexGlobals
{
	mat4 camToNDC;
	mat4 worldToCam;
	mat4 modelToWorld;
	mat4 modelNToWorldN;
};

out vec4 world_position;
out vec4 world_normal;

out vec4 model_pos;
out vec2 uv_frag;


void main() {
	uv_frag = uv_vert;
	model_pos = position;

	world_normal = modelNToWorldN * normal;
	world_position = modelToWorld * position;
	gl_Position = camToNDC * worldToCam * world_position;
}
