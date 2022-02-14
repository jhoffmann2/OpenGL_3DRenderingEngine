#include ../Include/ShaderGlobals.glsl

float unlerp(float value, float low, float high)
{
    return (value - low) / (high - low);
}

float GetLightDepth(float ndcDepth)
{
    return unlerp(ndcDepth, depth_range.x, depth_range.y);
}