
#include lightingUniforms.glsl
#include environmentMap.glsl

vec3 reflectedColor(vec3 P, vec3 N)
{
	const vec3 V = normalize(eyePos - P);
	const float ndotv = dot(N, V);
	const vec3 R = 2 * N * ndotv - V;

	return EnvironmentMap(R);
}