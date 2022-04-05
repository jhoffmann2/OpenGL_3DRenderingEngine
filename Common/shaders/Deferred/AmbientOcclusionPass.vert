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
in vec2 uv_vert;

out vec2 uv_frag;

void main() {
	gl_Position = position;
	uv_frag = uv_vert;
}
