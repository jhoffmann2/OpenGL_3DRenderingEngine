#include lightingUniforms.glsl
#include ShaderGlobals.glsl
#include shadow.glsl
#include uv.glsl
#define PI 3.14159265359f
#define TWO_PI 6.28318530718f

layout (location = 7) uniform sampler2D environmentTex;
layout (location = 8) uniform sampler2D environmentIrradianceTex;

float D(vec3 N, vec3 H, float alpha)
{
    return ((alpha + 2) / (TWO_PI)) * pow(dot(N, H), alpha);
}

vec3 F(vec3 L, vec3 H, vec3 ks)
{
    return ks + (vec3(1,1,1) - ks) * (1 - pow(dot(L, H), 5));
}

float G(vec3 L, vec3 V, vec3 H, vec3 N, float alpha)
{
    float theta = clamp(dot(V,N), -1.f, 1.f);
    float tan_theta = tan(theta);
    if(tan_theta  == 0.f)
        return 1.f;
    float a = (sqrt(alpha / 2.f + 1.f) / tan_theta);
    if(a < 1.6f)
        return 1.f;
    return (3.535f * a + 2.181f * a * a) / (1.f + 2.276f * a + 2.577f * a * a);
}

vec3 specularLight(vec3 N, vec3 H, vec3 L, vec3 ks_total, float ns, float theta)
{
    return 0.25f * D(N, H, ns) * F(L, H, ks_total) * (1.f / (theta * theta));
}

vec3 PseudoRand_SpecDir(uint i, float ns)
{
    const vec2 rand = hammersley[i].xy;
    const float theta = acos(pow(rand[0], 1.f / (ns + 1)));
    const float phi = TWO_PI * rand[1];

    return vec3(
        cos(phi) * sin(theta),
        sin(phi) * sin(theta),
        cos(theta)
    );
}

vec3 EnvironmentSpecular(vec3 V, vec3 N, vec3 ks, float ns)
{
    vec3 total = vec3(0,0,0);
    const uint count = specularSamplingLevel;

    const vec3 R = 2 * dot(N,V) * N - V;

    for(uint i = 0; i < count; ++i)
    {
        const vec3 C = normalize(PseudoRand_SpecDir(i, ns));
        const vec3 A = normalize(vec3(C.z, 0, -C.x));
        const vec3 B = normalize(cross(C,A));
        const mat3 M = inverse(mat3(A,B,C));

        vec3 L = normalize(M * R);
        const vec2 environmentUV = sphericalTextureMap(L);

        const vec3 H = normalize(L + V);

        const ivec2 envSize = textureSize(environmentTex, 0);
        const float lod = 0.5f * log2(envSize.x * envSize.y / count) -
                          0.5f * log2(D(N, H, ns) / 4.f);
        const vec3 lightColor = textureLod(environmentTex, environmentUV, lod).rgb;
        const float alpha = ns;
        const float ndotl = clamp(dot(N,L), -1, 1);
        const float ndotv = clamp(dot(N,V), -1, 1);
        const float ldoth = clamp(dot(L,H), -1, 1);
        const vec3 brdf = (G(L,V,H,N,alpha) * F(L, H, ks * ks)) / (4 * ndotl * ndotv);
        total += lightColor * brdf * ndotl;
    }
    return total / count;
}

vec3 brdfLight(vec3 P, vec3 N, vec3 kd_tex, vec3 ks_tex, uint materialIndex)
{
    if (materialIndex >= materials.length())
    return vec3(0.f);

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
    i_local += EmissiveColor;
    i_local += globalAmbientColor * ka;

    // take light from the environment
    const vec2 environmentUV = sphericalTextureMap(N);

    i_local += environmentLightStrength
                * (kd_total / PI)
                * vec3(texture(environmentIrradianceTex, environmentUV));

    i_local += environmentLightStrength * EnvironmentSpecular(V, N, ks_total, ns);

    for (int i = 0; i < lights.length(); ++i)
    {
        if (!lights[i].isActive)
        continue;

        float G = 0.f;

        // first light casts shadows
        if(i == 0)
        {
            G = shadow(vec4(P, 1));

            if(G >= 1)
            {
                continue;
            }
        }

        const vec3 L = (lights[i].type == 1) ?
        normalize(lights[i].direction) :
        normalize(lights[i].position - P);

        const float ndotl = dot(N, L);
        const vec3 H = normalize(L + V);

        const float Llen = length(lights[i].position - P);
        const float Attenuation = min(
        1/(
        lightAttenuation[0] +
        lightAttenuation[1] * Llen +
        lightAttenuation[2] * Llen * Llen
        ),
        1.0
        );

        const float LdotDir = dot(L, normalize(lights[i].direction));

        float SpotlightEffect =
        (lights[i].type == 2) ?
        (LdotDir - cos(radians(lights[i].spotlightOuterAngle))) /
        (cos(radians(lights[i].spotlightInnerAngle)) -
        cos(radians(lights[i].spotlightOuterAngle))) :
        1;
        SpotlightEffect = pow(SpotlightEffect, lights[i].spotlightFalloff);
        SpotlightEffect = clamp(SpotlightEffect, 0, 1);

        vec3 i_effect = { 0, 0, 0 }; // the effect that a single light source has on the object
        i_effect += lights[i].ambientColor * ka; // ambient
        i_effect += kd_total / PI; // diffuse
        i_effect += specularLight(N, H, L, ks_total, ns, ndotl); // specular

        i_effect *= lights[i].diffuseColor; // light illumination/color
        i_effect *= SpotlightEffect; // spotlight
        i_effect *= max(ndotl, 0); // light angle
        i_effect *= (lights[i].type == 1)? 1 : Attenuation;// Attenuation

        i_local += (1 - G) * min(i_effect, vec3(1,1,1));
    }

    return i_local;
}