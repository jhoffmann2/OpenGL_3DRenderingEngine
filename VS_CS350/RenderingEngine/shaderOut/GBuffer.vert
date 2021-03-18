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
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv_vert;
layout(location = 3) in float v_mat;
layout (std140, binding = 2) uniform VertexGlobals
{
	mat4 camToNDC;
	mat4 worldToCam;
	mat4 modelToWorld;
	mat4 modelNToWorldN;
};
out vec3 world_position;
out vec3 world_normal;
out vec3 model_pos;
out vec2 uv_frag;
out float local_mat;
void main() {
	uv_frag = uv_vert;
	model_pos = position;
	world_normal = (modelNToWorldN * vec4(normal, 0)).xyz;
	world_position = (modelToWorld * vec4(position, 1)).xyz;
	gl_Position = camToNDC * worldToCam * vec4(world_position, 1);
	local_mat = v_mat;
}
