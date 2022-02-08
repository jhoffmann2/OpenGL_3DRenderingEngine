/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SolidShading.vert
Purpose: renders geometric solid using fragment shading
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 11/03/2020
End Header --------------------------------------------------------*/
#version 460
#include ../Include/MeshProperties.glsl

out float depth;

float unlerp(float value, float low, float high)
{
    return (value - low) / (high - low);
}

void main() {
    gl_Position = camToNDC * (worldToCam * (modelToWorld * vec4(position, 1)));
    const vec3 ndc_position = (gl_Position / gl_Position.w).xyz;

    const float scene_radius = 0.5 * length(sceneBoundsMax - sceneBoundsMin);
    const vec4 scene_center = 0.5 * (sceneBoundsMax + sceneBoundsMin);
    const float cam_world_distance = length(scene_center - camToWorld[3]);
    const vec4 cam_forward = -normalize(camToWorld[2]);
    const vec4 world_scene_near = camToWorld[3] + ((cam_world_distance - scene_radius) * cam_forward);
    const vec4 world_scene_far = camToWorld[3] + ((cam_world_distance + scene_radius) * cam_forward);

    const vec4 ndc_scene_near = (camToNDC * (worldToCam * world_scene_near));
    const vec4 ndc_scene_far = (camToNDC * (worldToCam * world_scene_far));

    const float min_depth = ndc_scene_near.z / abs(ndc_scene_near.w);
    const float max_depth = ndc_scene_far.z / abs(ndc_scene_far.w);

    depth = unlerp(ndc_position.z, min_depth, max_depth);
}
