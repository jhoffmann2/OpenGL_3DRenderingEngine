#include "ShaderGlobalSystem.h"
#include <algorithm>

ShaderGlobalSystem &ShaderGlobalSystem::Instance()
{
  static ShaderGlobalSystem system;
  return system;
}

void ShaderGlobalSystem::Init()
{
  ShaderGlobalSystem & instance = Instance();
  glGenBuffers(1, &instance.uboBlock_);
  glBindBuffer(GL_UNIFORM_BUFFER, instance.uboBlock_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(shaderData_), &instance.shaderData_, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferBase(GL_UNIFORM_BUFFER, ShaderData::binding_, instance.uboBlock_);
}

void ShaderGlobalSystem::SetCamToNDC(const glm::mat4& mat)
{
  ShaderGlobalSystem & instance = Instance();
  glm::mat4& element = instance.shaderData_.camToNDC_;
  element = mat;
  WriteToGPU(&element);
}

void ShaderGlobalSystem::SetCamToWorld(const glm::mat4& mat)
{
  ShaderGlobalSystem & instance = Instance();

  glm::mat4& camToWorld = instance.shaderData_.camToWorld_;
  glm::mat4& worldToCam = instance.shaderData_.worldToCam_;
  worldToCam = glm::inverse(mat);
  camToWorld = mat;

  WriteToGPU(&worldToCam, 2 * sizeof(glm::mat4));
}

void ShaderGlobalSystem::SetModelToWorld(const glm::mat4& mat)
{
  ShaderGlobalSystem & instance = Instance();
  glm::mat4& m2w = instance.shaderData_.modelToWorld_;
  glm::mat4& m2w_normal = instance.shaderData_.modelNToWorldN_;
  m2w = mat;
  m2w_normal = transpose(inverse(m2w));

  WriteToGPU(&m2w, 2 * sizeof(glm::mat4));
}

void ShaderGlobalSystem::SetupForShadowMap(const Camera &cam, const ntg::bounds3 &sceneBounds)
{
  ShaderGlobalSystem & instance = Instance();

  cam.UpdateGPUCamera();
  ShaderGlobalSystem::SetShadowWorldToNDC(
      ShaderGlobalSystem::GetCamToNDC() * ShaderGlobalSystem::GetWorldToCam()
  );

  const float scene_radius = glm::max(glm::length(sceneBounds.half_size()), 1.f);
  const float cam_world_distance = length(sceneBounds.center() - cam.eye());
  const glm::vec4 cam_forward = glm::vec4(-cam.back(), 0);
  const glm::vec4 world_scene_near = glm::vec4(cam.eye(), 1) + ((cam_world_distance - scene_radius) * cam_forward);
  const glm::vec4 world_scene_far = glm::vec4(cam.eye(), 1) + ((cam_world_distance + scene_radius) * cam_forward);

  const glm::vec4 ndc_scene_near = (instance.shaderData_.shadowWorldToNDC_() * world_scene_near);
  const glm::vec4 ndc_scene_far = (instance.shaderData_.shadowWorldToNDC_() * world_scene_far);

  glm::vec2& element = instance.shaderData_.depth_range;
  element[0] = ndc_scene_near.z / abs(ndc_scene_near.w);
  element[1] = ndc_scene_far.z / abs(ndc_scene_far.w);

  WriteToGPU(&element);
}

void ShaderGlobalSystem::SetShadowWorldToNDC(const glm::mat4 &mat)
{
  ShaderGlobalSystem & instance = Instance();
  glm::mat4& element = instance.shaderData_.shadowWorldToNDC_;
  element = mat;
  WriteToGPU(&element);
}

const glm::mat4&ShaderGlobalSystem::GetCamToNDC()
{
  return Instance().shaderData_.camToNDC_;
}

const glm::mat4&ShaderGlobalSystem::GetWorldToCam()
{
  return Instance().shaderData_.worldToCam_;
}

const glm::mat4&ShaderGlobalSystem::GetModelToWorld()
{
  return Instance().shaderData_.modelToWorld_;
}

const glm::mat4&ShaderGlobalSystem::GetModelNToWorldN()
{
  return Instance().shaderData_.modelNToWorldN_;
}

ShaderGlobalSystem::ShaderGlobalSystem() = default;
