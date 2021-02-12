
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