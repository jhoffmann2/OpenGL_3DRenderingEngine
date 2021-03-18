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
vec2 planarTextureMap(vec3 pos)
{
	return pos.xy;
}
vec2 cylindricalTextureMap(vec3 pos)
{
	float theta = degrees(atan(pos.z, pos.x));
	return vec2(theta / 360, pos.y);
}
vec2 sphericalTextureMap(vec3 pos)
{
	float theta = degrees(atan(pos.z, pos.x));
	float r = length(pos);//sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
	float phi = degrees(acos(pos.y / r));
	
	return vec2(theta / 360, phi / 180);
}
vec2 cubicTextureMap(vec3 r, out uint cubeFace)
{
	r = normalize(r);
	if(abs(r.x) > abs(r.y) && abs(r.x) > abs(r.z))
	{
		cubeFace = (r.x > 0)? 0 : 2;
		r *= 1.f / abs(r.x);
		r /= 2;
		r += vec4(0.5, 0.5, 0.5, 0);
		return r.yz;
	}
	else if(abs(r.y) > abs(r.x) && abs(r.y) > abs(r.z))
	{
		cubeFace = (r.y > 0)? 4 : 5;
		r *= 1.f / abs(r.y);
		r /= 2;
		r += vec4(0.5, 0.5, 0.5, 0);
		return r.xz;
	}
	else if(abs(r.z) > abs(r.x) && abs(r.z) > abs(r.y))
	{
		cubeFace = (r.z > 0)? 1 : 3;
		r *= 1.f / abs(r.z);
		r /= 2;
		r += vec4(0.5, 0.5, 0.5, 0);
		return r.xy;
	}
}
vec2 calculateUV(vec3 pos, vec2 uv_frag, int textureMode_)
{
	switch(textureMode_)
	{
	case 0: // CPU
		return uv_frag;
	case 1: // GPU_PLANAR
		return planarTextureMap(pos);
	case 2: // GPU_CYLINDRICAL
		return cylindricalTextureMap(pos);
	case 3: // GPU_SPHYRICAL
		return sphericalTextureMap(pos);
	}
}
in vec3 world_position;
in vec3 world_normal;
in vec3 model_pos;
in vec2 uv_frag;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
layout (location = 0) out vec4 DiffuseOut;
layout (location = 1) out vec4 WorldPosOut;
layout (location = 2) out vec4 NormalOut;
void main(void)
{
	const vec2 uv = calculateUV(model_pos, uv_frag, materials[curMaterial].textureMode_);
	DiffuseOut.rgb = texture(diffuseTexture, uv).rgb;
	DiffuseOut.a = texture(specularTexture, uv).r;
	WorldPosOut.xyz = world_position;
	WorldPosOut.w = float(curMaterial) / 10;
	NormalOut.xyz = normalize(world_normal);
}
