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
void main() {
	gl_Position = camToNDC * worldToCam * modelToWorld * vec4(position, 1);
}
