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

void ShaderGlobalSystem::SetSceneBounds(const ntg::bounds3 &bounds)
{
  ShaderGlobalSystem & instance = Instance();

  glm::vec4& boundsMin = instance.shaderData_.sceneBoundsMin_;
  glm::vec4& boundsMax = instance.shaderData_.sceneBoundsMax_;
  boundsMin = glm::vec4(bounds.min, 1);
  boundsMax = glm::vec4(bounds.max, 1);

  WriteToGPU(&boundsMin, 2 * sizeof(glm::vec4));
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
