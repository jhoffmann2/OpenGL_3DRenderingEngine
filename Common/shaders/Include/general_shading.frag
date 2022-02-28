in vec3 model_pos;
in vec2 uv_frag;

in vec3 world_position;
in vec3 world_normal;

layout (location = 5) uniform sampler2D diffuseTexture;
layout (location = 6) uniform sampler2D specularTexture;

out vec4 frag_color;


void main(void) 
{
	const vec2 uv = calculateUV(model_pos, uv_frag, materials[curMaterial].textureMode_);
	const vec3 kd_tex = texture(diffuseTexture, uv).rgb;
	const vec3 ks_tex = texture(specularTexture, uv).rgb;

	const vec3 N = normalize(world_normal);
	
	vec3 i_local = shading_method(world_position, N, kd_tex, ks_tex, curMaterial);

	const float Vlen = length(eyePos - world_position);
	float S = clamp(((fogFar - Vlen) / (fogFar - fogNear)), 0, 1);

	frag_color = vec4(mix(fogColor, i_local, S), 1);
}