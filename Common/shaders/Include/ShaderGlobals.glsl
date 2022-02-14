layout (std140, binding = 2) uniform ShaderGlobals
{
	mat4 camToNDC;
	mat4 worldToCam;
	mat4 camToWorld;
	mat4 modelToWorld;
	mat4 modelNToWorldN;
	mat4 shadowWorldToNDC;
	vec2 depth_range;
};