
vec3 ToneMap(vec3 color, float exposure)
{
    return vec3(
        pow((exposure * color.r) / (exposure * color.r + 1), contrast/2.2),
        pow((exposure * color.g) / (exposure * color.g + 1), contrast/2.2),
        pow((exposure * color.b) / (exposure * color.b + 1), contrast/2.2)
    );
}