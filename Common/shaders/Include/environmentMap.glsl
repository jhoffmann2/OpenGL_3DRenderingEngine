
#include uv.glsl

layout(location = 0) uniform sampler2D environment0;
layout(location = 1) uniform sampler2D environment1;
layout(location = 2) uniform sampler2D environment2;
layout(location = 3) uniform sampler2D environment3;
layout(location = 4) uniform sampler2D environment4;
layout(location = 5) uniform sampler2D environment5;

vec3 EnvironmentMap(vec4 direction)
{
  uint cubeFace;
	vec2 cubeUV = cubicTextureMap(direction, cubeFace);
	switch(cubeFace)
	{
	case 0:
		return texture(environment0, cubeUV).xyz;
	case 1:
		return texture(environment1, cubeUV).xyz;
	case 2:
		return texture(environment2, cubeUV).xyz;
	case 3:
		return texture(environment3, cubeUV).xyz;
	case 4:
		return texture(environment4, cubeUV).xyz;
	case 5:
		return texture(environment5, cubeUV).xyz;
	}
}