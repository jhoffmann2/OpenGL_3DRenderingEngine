/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongLighting.vert
Purpose: renders mesh with phong lighting
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 11/3/2020
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
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv_vert;
layout (std140, binding = 2) uniform VertexGlobals
{
	mat4 camToNDC;
	mat4 worldToCam;
	mat4 modelToWorld;
	mat4 modelNToWorldN;
};
vec3 phongLight(vec3 P, vec3 N, vec3 kd_tex, vec3 ks_tex, uint materialIndex)
{
	if(materialIndex >= materials.length())
		return vec3(0.f);
	const vec3 V = normalize(eyePos - P);
	const Material material = materials[materialIndex];
	const vec3 EmissiveColor = material.EmissiveColor;
	const vec3 ka = material.ka;
	const vec3 kd = material.kd;
	const vec3 ks = material.ks;
	const float ns = material.ns;
	const vec3 kd_total = kd * kd_tex;
	const vec3 ks_total = ks * ks_tex;
	vec3 i_local = vec3(0,0,0);
	i_local += EmissiveColor;
	i_local += globalAmbientColor * ka;
	
	for(int i = 0; i < lights.length(); ++i)
	{
		if(!lights[i].isActive)
			continue;
		const vec3 L = (lights[i].type == 1) ? 
			normalize(lights[i].direction) : 
			normalize(lights[i].position - P);
		const float ndotl = dot(N, L);
		const vec3 R = 2 * N * ndotl - L;
		const float rdotv = dot(R, V);
		const float Llen = length(lights[i].position - P);
		const float Attenuation = min(
				1/(
					lightAttenuation[0] + 
					lightAttenuation[1] * Llen + 
					lightAttenuation[2] * Llen * Llen
				),
				1.0
			);
		const float LdotDir = dot(L, normalize(lights[i].direction));
		float SpotlightEffect = 
			(lights[i].type == 2) ?
				(LdotDir - cos(radians(lights[i].spotlightOuterAngle))) / 
					(cos(radians(lights[i].spotlightInnerAngle)) - 
					cos(radians(lights[i].spotlightOuterAngle))) :
				1;
		SpotlightEffect = pow(SpotlightEffect, lights[i].spotlightFalloff);
		SpotlightEffect = clamp(SpotlightEffect, 0, 1);
		vec3 i_effect = {0,0,0}; // the effect that a single light source has on the object
		i_effect += lights[i].ambientColor * ka;													 // ambient
		i_effect += SpotlightEffect * (lights[i].diffuseColor * kd_total * max(ndotl, 0));			 // diffuse
		i_effect += SpotlightEffect * (lights[i].specularColor * ks_total * pow(max(rdotv, 0), ns)); // specular
		i_effect *= (lights[i].type == 1)? 1 : Attenuation;											 // Attenuation
		i_local += i_effect;
	}
	return i_local;
}
vec3 lighting_method(vec3 P, vec3 N, vec3 kd_tex, vec3 ks_tex, uint materialIndex)
{
	return phongLight(P,N,kd_tex,ks_tex,materialIndex);
}
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
out vec4 color;
void main() 
{
	vec3 world_normal = (modelNToWorldN * vec4(normal, 0)).xyz;
	vec3 world_position = (modelToWorld * vec4(position, 1)).xyz;
	gl_Position = camToNDC * worldToCam * vec4(world_position, 1);
	const vec2 uv = calculateUV(position, uv_vert, materials[curMaterial].textureMode_);
	const vec3 kd_tex = texture(diffuseTexture, uv).rgb;
	const vec3 ks_tex = texture(specularTexture, uv).rgb;
	const vec3 N = normalize(world_normal);
	vec3 i_local = lighting_method(world_position, N, kd_tex, ks_tex, curMaterial);
	const float Vlen = length(eyePos - world_position);
	float S = clamp(((fogFar - Vlen) / (fogFar - fogNear)), 0, 1);
	color = vec4((S * i_local) + ((1-S) * fogColor), 1);
}
