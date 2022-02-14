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

in float depth;
layout (location = 3) out vec4 DepthMap;

void main(void) {
	DepthMap.r = depth;
	DepthMap.g = DepthMap.r * depth;
	DepthMap.b = DepthMap.g * depth;
	DepthMap.a = DepthMap.b * depth;
}
