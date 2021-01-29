
#include lightingUniforms.glsl
#include environmentMap.glsl

vec3 reflectedColor(vec4 P, vec4 N)
{
	const vec4 V = normalize(eyePos - P);
	const float ndotv = dot(N, V);
	const vec4 R = 2 * N * ndotv - V;

	return EnvironmentMap(R);
}