#include "MaterialSystem.h"

MaterialSystem& MaterialSystem::Instance()
{
  static MaterialSystem system;
  return system;
}

void MaterialSystem::Init()
{
  MaterialSystem &instance = Instance();
  glGenBuffers(1, &instance.ssbo_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, instance.ssbo_);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shaderData_), &instance.shaderData_, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ShaderData::binding_, instance.ssbo_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void MaterialSystem::Update()
{
  MaterialSystem& instance = Instance();
  if (instance.dirtyMats_.any())
  {
    for (size_t i = 0; i < instance.dirtyMats_.size(); ++i)
    {
      if (instance.dirtyMats_[i])
      {
        const GLbyte* data = reinterpret_cast<GLbyte*>(&GetRawMaterialData(i));
        const GLbyte* begin = reinterpret_cast<GLbyte*>(&instance.shaderData_);
        const size_t offset = data - begin;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instance.ssbo_);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), data);
      }
    }

    // unbind buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    instance.dirtyMats_ = { false }; // clear dirty materials
  }
}

void MaterialSystem::SetDirty(const Material& mat)
{
  Instance().dirtyMats_[MaterialIndex(mat)] = true;
}

void MaterialSystem::SetDirty(size_t index)
{
  Instance().dirtyMats_[index] = true;
}

void MaterialSystem::SetCurrentMaterialIndex(size_t index)
{
  MaterialSystem& instance = Instance();
  auto& cur = instance.shaderData_.curMaterial_;
  if(cur != static_cast<GLuint>(index))
  {
    cur = static_cast<GLuint>(index);

    // upload material index to gpu
    const GLbyte* data = reinterpret_cast<GLbyte*>(&cur);
    const GLbyte* begin = reinterpret_cast<GLbyte*>(&instance.shaderData_);
    const size_t offset = data - begin;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, instance.ssbo_);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(cur), data);
  }

}

const Material& MaterialSystem::GetCurrentMaterial()
{
  return Instance().shaderData_.materials_[GetCurrentMaterialIndex()];
}

size_t MaterialSystem::GetCurrentMaterialIndex()
{
  return Instance().shaderData_.curMaterial_;
}

Material& MaterialSystem::GetRawMaterialData(size_t index)
{
  return Instance().shaderData_.materials_[index];
}

size_t MaterialSystem::MaterialIndex(const Material& material)
{
  // use the pointer offset to calculate the index of the material
  return reinterpret_cast<const AlignData<16, Material>*>(&material) - &Instance().shaderData_.materials_[0];
}


