#include "VertexGlobalSystem.h"

#include "Affine.h"


VertexGlobalSystem& VertexGlobalSystem::Instance()
{
  static VertexGlobalSystem system;
  return system;
}

void VertexGlobalSystem::Init()
{
  VertexGlobalSystem& instance = Instance();
  glGenBuffers(1, &instance.uboBlock_);
  glBindBuffer(GL_UNIFORM_BUFFER, instance.uboBlock_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(shaderData_), &instance.shaderData_, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferBase(GL_UNIFORM_BUFFER, ShaderData::binding_, instance.uboBlock_);
}

void VertexGlobalSystem::SetCamToNDC(const glm::mat4& mat)
{
  VertexGlobalSystem& instance = Instance();
  glm::mat4& element = instance.shaderData_.camToNDC_;
  element = mat;
  WriteToGPU(&element);
}

void VertexGlobalSystem::SetWorldToCam(const glm::mat4& mat)
{
  VertexGlobalSystem& instance = Instance();
  glm::mat4& element = instance.shaderData_.worldToCam_;
  element = mat;
  WriteToGPU(&element);
}

void VertexGlobalSystem::SetModelToWorld(const glm::mat4& mat)
{
  VertexGlobalSystem& instance = Instance();
  glm::mat4& m2w = instance.shaderData_.modelToWorld_;
  glm::mat4& m2w_normal = instance.shaderData_.modelNToWorldN_;
  m2w = mat;
  m2w_normal = transpose(affineInverse(m2w));

  WriteToGPU(&m2w, 2 * sizeof(glm::mat4));
}

const glm::mat4& VertexGlobalSystem::GetCamToNDC()
{
  return Instance().shaderData_.camToNDC_;
}

const glm::mat4& VertexGlobalSystem::GetWorldToCam()
{
  return Instance().shaderData_.worldToCam_;
}

const glm::mat4& VertexGlobalSystem::GetModelToWorld()
{
  return Instance().shaderData_.modelToWorld_;
}

const glm::mat4& VertexGlobalSystem::GetModelNToWorldN()
{
  return Instance().shaderData_.modelNToWorldN_;
}

VertexGlobalSystem::VertexGlobalSystem() = default;
