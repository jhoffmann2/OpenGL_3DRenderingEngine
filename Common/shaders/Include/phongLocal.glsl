#include lightingUniforms.glsl
in vec3 light_pos;
in float light_radius;
#define PI 3.1415926538

float SinEaseInOut(float t)
{
    t = clamp(t, 0, 1);
    return .5 * (1 + sin(PI * (t - 0.5)));
}

float ExpEaseIn(float t, float exp)
{
    t = clamp(t, 0, 1);
    return pow(t, exp);
}

vec3 phongLocal(vec3 P, vec3 N, vec3 kd_tex, vec3 ks_tex, uint materialIndex, uint lightMaterialIndex)
{
    if (materialIndex >= materials.length())
    {
        return vec3(0.f);
    }

    const vec3 V = normalize(eyePos - P);

    const Material material = materials[materialIndex];
    const vec3 EmissiveColor = material.EmissiveColor;
    const vec3 ka = material.ka;
    const vec3 kd = material.kd;
    const vec3 ks = material.ks;
    const float ns = material.ns;

    const vec3 kd_total = kd * kd_tex;
    const vec3 ks_total = ks * ks_tex;

    vec3 i_local = vec3(0, 0, 0);

    const Material lightMaterial = materials[lightMaterialIndex];
    const vec3 lightAmbient = lightMaterial.ka;
    const vec3 lightDiffuse = lightMaterial.kd;
    const vec3 lightSpecular = lightMaterial.ks;

    const float Llen = length(light_pos - P);
    const vec3 L = (light_pos - P) / Llen;

    const float ndotl = dot(N, L);
    const vec3 R = 2 * N * ndotl - L;
    const float rdotv = dot(R, V);
    const float Attenuation = min(
    1/(
    lightAttenuation[0] +
    lightAttenuation[1] * Llen +
    lightAttenuation[2] * Llen * Llen
    ),
    1.0
    );

    vec3 i_effect = { 0, 0, 0 };// the effect that a single light source has on the object
    i_effect += lightAmbient * ka;// ambient
    i_effect += (lightDiffuse * kd_total * max(ndotl, 0));// diffuse
    i_effect += (lightSpecular * ks_total * pow(max(rdotv, 0), ns));// specular
    i_effect *= Attenuation;// Attenuation

    i_local += i_effect;

    return mix(i_local, vec3(0), SinEaseInOut(ExpEaseIn(Llen / light_radius, 4)));
}
