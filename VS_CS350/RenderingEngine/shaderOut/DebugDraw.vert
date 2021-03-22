/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LineRender.vert
Purpose: renders lines
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/ 
#version 460
in vec3 position;
layout (std140, binding = 2) uniform VertexGlobals
{
	mat4 camToNDC;
	mat4 worldToCam;
	mat4 modelToWorld;
	mat4 modelNToWorldN;
};
out vec4 worldPos;
void main() {
	worldPos = modelToWorld * vec4(position, 1);
	gl_Position = camToNDC * worldToCam * worldPos;
}
