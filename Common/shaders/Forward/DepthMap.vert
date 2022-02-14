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
#include ../Include/lightDepth.glsl

out float depth;

void main() {
    gl_Position = camToNDC * (worldToCam * (modelToWorld * vec4(position, 1)));
    const vec3 ndc_position = (gl_Position / gl_Position.w).xyz;

    depth = GetLightDepth(ndc_position.z);
}
