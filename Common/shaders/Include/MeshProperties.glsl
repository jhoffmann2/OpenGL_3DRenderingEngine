
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uv_vert;

layout (std140, binding = 2) uniform VertexGlobals
{
	mat4 camToNDC;
	mat4 worldToCam;
	mat4 modelToWorld;
	mat4 modelNToWorldN;
};