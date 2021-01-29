#include "MaterialHandle.h"

MaterialHandle MaterialHandle::null()
{
  return MaterialHandle();
}

MaterialHandle::MaterialHandle() : index_(std::numeric_limits<size_t>::max())
{
}

MaterialHandle::MaterialHandle(size_t index) : index_(index)
{
  assert(index < MaterialSystem::materialCount);
}

MaterialHandle& MaterialHandle::operator=(const MaterialHandle other)
{
  SetIndex(other.index_);
  return *this;
}

const glm::vec3& MaterialHandle::GetEmissiveColor() const
{
  return getData().EmissiveColor_;
}

const glm::vec3& MaterialHandle::GetDiffuseColor() const
{
  return getData().kd_;
}

const glm::vec3& MaterialHandle::GetSpecularColor() const
{
  return getData().ks_;
}

const glm::vec3& MaterialHandle::GetAmbientColor() const
{
  return getData().ka_;
}

float MaterialHandle::GetSpecularExponent() const
{
  return getData().ns_;
}

MaterialHandle::TextureMode MaterialHandle::GetTextureMode() const
{
  return getData().textureMode_;
}

size_t MaterialHandle::GetIndex() const
{
  return index_;
}

void MaterialHandle::SetEmissiveColor(const glm::vec3& color)
{
  Material& data = getData();
  auto& curr = data.EmissiveColor_;
  if (curr != color)
  {
    curr = color;
    MaterialSystem::SetDirty(index_);
  }
}

void MaterialHandle::SetDiffuseColor(const glm::vec3& color)
{
  Material& data = getData();
  auto& curr = data.kd_;
  if (curr != color)
  {
    curr = color;
    MaterialSystem::SetDirty(index_);
  }
}

void MaterialHandle::SetSpecularColor(const glm::vec3& color)
{
  Material& data = getData();
  auto& curr = data.ks_;
  if (curr != color)
  {
    curr = color;
    MaterialSystem::SetDirty(index_);
  }
}

void MaterialHandle::SetAmbientColor(const glm::vec3& color)
{
  Material& data = getData();
  auto& curr = data.ka_;
  if (curr != color)
  {
    curr = color;
    MaterialSystem::SetDirty(index_);
  }
}

void MaterialHandle::SetSpecularExponent(float exp)
{
  Material& data = getData();
  auto& curr = data.ns_;
  if (curr != exp)
  {
    curr = exp;
    MaterialSystem::SetDirty(index_);
  }
}

void MaterialHandle::SetTextureMode(TextureMode mode)
{
  Material& data = getData();
  auto& curr = data.textureMode_;
  if (curr != mode)
  {
    curr = mode;
    MaterialSystem::SetDirty(index_);
  }
}

void MaterialHandle::SetIndex(size_t index)
{
  index_ = index;
}

void MaterialHandle::Clone(const MaterialHandle other)
{
  SetEmissiveColor(other.GetEmissiveColor());
  SetAmbientColor(other.GetAmbientColor());
  SetDiffuseColor(other.GetDiffuseColor());
  SetSpecularColor(other.GetSpecularColor());
  SetSpecularExponent(other.GetSpecularExponent());
  SetTextureMode(other.GetTextureMode());
}

Material& MaterialHandle::getData() const
{
  return MaterialSystem::GetRawMaterialData(index_);
}
