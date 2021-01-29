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

struct Light
{
	vec4 position;
	vec4 direction;

	vec3 diffuseColor;
	vec3 specularColor;
	vec3 ambientColor;

	int type;
	float spotlightInnerAngle;
	float spotlightOuterAngle;
	float spotlightFalloff;
	bool isActive;
};

layout (std140, binding = 2) uniform VertexGlobals
{
	mat4 camToNDC;
	mat4 worldToCam;
	mat4 modelToWorld;
	mat4 modelNToWorldN;
};

layout (std140, binding = 0) uniform Lighting
{
	vec3 lightAttenuation;
    vec3 fogColor;
	float fogNear;
	float fogFar;
    vec3 globalAmbientColor;
	vec4 eyePos;
    Light lights[16];
}; 

in vec4 position;

out vec4 camRay;


void main() {

	vec4 worldPos = modelToWorld * position;
	gl_Position = camToNDC * worldToCam * worldPos;

	camRay = worldPos - eyePos;
}
