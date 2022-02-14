#include lightingUniforms.glsl
#include ShaderGlobals.glsl
#include lightDepth.glsl
layout (location = 4) uniform sampler2D shadowMap;


vec3 cholesky(float m11, float m12, float m13, float m22, float m23, float m33, float z1, float z2, float z3)
{
    const float a = sqrt(m11);
    const float b = m12 / a;
    const float c = m13 / a;
    const float d = sqrt(m22 - (b * b));
    const float e = (m23 - (b * c)) / d;
    const float f = sqrt(m33 - (c * c) - (e * e));

    const float cp1 = z1 / a;
    const float cp2 = (z2 - (b * cp1)) / d;
    const float cp3 = (z3 - (c * cp1) - (e * cp2)) / f;

    const float c3 = cp3 / f;
    const float c2 = (cp2 - (e * c3)) / d;
    const float c1 = (cp1 - (b * c2) - (c * c3)) / a;

    return vec3(c1, c2, c3);
}

float shadow(vec4 P)
{
    vec4 shadowNDC = shadowWorldToNDC * P;
    shadowNDC /= shadowNDC.w;
    const vec2 shadowUV = (shadowNDC.xy + 1) * 0.5;
    const vec4 b = texture(shadowMap, shadowUV);
    const float d = GetLightDepth(shadowNDC.z);

    const float alpha = 0.001f;
    const vec4 bp = (1 - alpha) * b + alpha * vec4(.5f);

    const vec3 c = cholesky(
        // m
        1.f, bp[0], bp[1],
        bp[1], bp[2],
        bp[3],
        // zf
        1.f, d, (d * d)
    );

    float z2 = (sqrt(
        (c[1] * c[1]) - (4 * c[0] * c[2])
    ) - c[1]) / (2 * c[2]);

    float z3 = -(sqrt(
        (c[1] * c[1]) - (4 * c[0] * c[2])
    ) + c[1]) / (2 * c[2]);

    // ensure that z2 <= z3
    if(z3 < z2)
    {
        float temp = z2;
        z2 = z3;
        z3 = temp;
    }

    if(d <= z2)
        return 0;
    if(d <= z3)
        return   (d  * z3 - bp[0] * (d  + z3) + bp[1]) / ((z3 - z2)*(d - z2));
    return  1 - ((z2 * z3 - bp[0] * (z2 + z3) + bp[1]) / ((d  - z2)*(d - z3)));
}


vec3 phongLight(vec3 P, vec3 N, vec3 kd_tex, vec3 ks_tex, uint materialIndex)
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
        const vec3 R = 2 * N * ndotl - L;
        const float rdotv = dot(R, V);
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


        vec3 i_effect = { 0, 0, 0 };// the effect that a single light source has on the object
        i_effect += lights[i].ambientColor * ka;// ambient
        i_effect += SpotlightEffect * (lights[i].diffuseColor * kd_total * max(ndotl, 0));// diffuse
        i_effect += SpotlightEffect * (lights[i].specularColor * ks_total * pow(max(rdotv, 0), ns));// specular
        i_effect *= (lights[i].type == 1)? 1 : Attenuation;// Attenuation

        i_local += (1 - G) * i_effect;
    }

    return i_local;
}