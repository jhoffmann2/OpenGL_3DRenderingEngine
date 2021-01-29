
#include lightingUniforms.glsl
#include environmentMap.glsl

vec4 refractedVec(vec4 I, vec4 N, float eta)
{
  const float ndoti = dot(N, I);
  const float k = 1.0 - eta * eta * (1.0 - ndoti * ndoti);
  if (k < 0.0)
      return vec4(0,0,0,0);
  else
      return eta * I - (eta * ndoti + sqrt(k)) * N;
}

vec3 refractedColor(vec4 V, vec4 N, vec3 eta)
{
  vec3 color;
	color.r = EnvironmentMap(refractedVec(-V, N, eta.r)).r;
	color.g = EnvironmentMap(refractedVec(-V, N, eta.g)).g;
	color.b = EnvironmentMap(refractedVec(-V, N, eta.b)).b;

  return color;
}
