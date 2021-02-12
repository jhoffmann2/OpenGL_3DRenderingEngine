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
struct Light
{
	vec3 position;
	vec3 direction;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 ambientColor;
	int type;
	float spotlightInnerAngle;
	float spotlightOuterAngle;
	float spotlightFalloff;
	bool isActive;
};
layout (std140, binding = 0) buffer Lighting
{
	vec3 lightAttenuation;
	vec3 fogColor;
	float fogNear;
	float fogFar;
	vec3 globalAmbientColor;
	vec3 eyePos;
	Light lights[];
}; 
struct Material
{
	vec3 EmissiveColor;
	vec3 ka;
	vec3 kd;
	vec3 ks;
	float ns;
	int textureMode_;
};
layout (std140, binding = 1) buffer Materials
{
	uint curMaterial;
	Material materials[];
};
out vec4 frag_color;
void main(void) {
	frag_color = vec4(materials[curMaterial].EmissiveColor, 1);
}
