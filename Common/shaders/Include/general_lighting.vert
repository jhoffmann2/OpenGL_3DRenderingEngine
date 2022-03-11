layout (location = 5) uniform sampler2D diffuseTexture;
layout (location = 6) uniform sampler2D specularTexture;

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