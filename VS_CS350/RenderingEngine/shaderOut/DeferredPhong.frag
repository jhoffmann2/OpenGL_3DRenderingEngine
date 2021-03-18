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
uniform sampler2D diffuseTex;  // = { kd.r, kd.g, kd.b, ks }
uniform sampler2D worldPosTex; // = { P.x, P.y, P.z, materialIndex }
uniform sampler2D normalTex;   // = { N.x, N.y, N.z, 0 }
in vec2 uv_frag;
out vec4 frag_color;
void main(void)
{
	vec4 P = texture(worldPosTex, uv_frag);
	const float Vlen = length(eyePos.xyz - P.xyz);
	float S = clamp(((fogFar - Vlen) / (fogFar - fogNear)), 0, 1);
	if(S == 0) // break out early to avoid unneccesary calculations on pixels that are shrouded in fog
	{
		frag_color = vec4(fogColor , 1);
		return;
	}
	
	vec4 N = texture(normalTex, uv_frag);
	uint material = uint(P.w * 10);
	P.w = 1;
	vec4 kd = texture(diffuseTex, uv_frag);
	vec3 ks = vec3(kd.w);
	vec3 i_local = (S < 1)? phongLight(P.xyz, N.xyz, kd.rgb, ks, material) : vec3(0);
		
	frag_color = vec4(mix(fogColor, i_local, S), 1);
}
