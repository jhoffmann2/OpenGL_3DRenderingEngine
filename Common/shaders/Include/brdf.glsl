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
    return ks + (vec3(1,1,1) - ks) * pow(1 - dot(L, H), 5);
}

float V(vec3 L, vec3 H)
{
    float ldoth = dot(L, H);
    return 1.f / (ldoth * ldoth);
}

vec3 specularLight(vec3 N, vec3 H, vec3 L, vec3 ks_total, float ns)
{
    vec3 c = 0.25f * F(L, H, ks_total) * V(L, H) * D(N, H, ns);
    return clamp(c, vec3(0,0,0), ks_total);
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
    const vec3 A = normalize(vec3(R.z, 0, -R.x));
    const vec3 B = normalize(cross(R,A));

    for(uint i = 0; i < count; ++i)
    {
        vec3 L = PseudoRand_SpecDir(i, ns);
        L = normalize(L.x * A + L.y * B + L.z * R);
        const vec2 environmentUV = sphericalTextureMap(L);

        const vec3 H = normalize(L + V);

        const ivec2 envSize = textureSize(environmentTex, 0);
        const float lod = 0.5f * log2(envSize.x * envSize.y / count) -
                          0.5f * log2(D(N, H, ns) / 4.f);
        const vec3 lightColor = textureLod(environmentTex, environmentUV, lod).rgb;
        const float alpha = ns;
        const float ndotl = clamp(dot(N,L), -1, 1);
        total += lightColor * specularLight(N, H, L, ks, ns) * ndotl;
    }
    return total / count;
}

vec3 brdfLight(vec3 P, vec3 N, vec3 kd_tex, vec3 ks_tex, uint materialIndex, float ambientOcclusion)
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

    // take light from the environment
    const vec2 environmentUV = sphericalTextureMap(N);

    i_local += ambientOcclusion * (kd_total / PI) * vec3(texture(environmentIrradianceTex, environmentUV));

    i_local += ambientOcclusion * material.specularStrenth * EnvironmentSpecular(V, N, ks_total, ns);

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
        i_effect += material.specularStrenth * specularLight(N, H, L, ks_total, ns); // specular

        i_effect *= lights[i].diffuseColor; // light illumination/color
        i_effect *= SpotlightEffect; // spotlight
        i_effect *= max(ndotl, 0); // light angle
        i_effect *= (lights[i].type == 1)? 1 : Attenuation;// Attenuation

        i_local += (1 - G) * min(i_effect, vec3(1,1,1));
    }

    return i_local;
}