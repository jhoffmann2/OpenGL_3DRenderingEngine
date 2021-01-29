#include "LightHandle.h"

#include "Light.h"
#include "LightSystem.h"

LightHandle LightHandle::null()
{
  return LightHandle();
}

LightHandle::LightHandle() : index_(std::numeric_limits<size_t>::max())
{
}

LightHandle::LightHandle(size_t index) : index_(index)
{
  assert(index < LightSystem::lightCount);
}

LightHandle& LightHandle::operator=(const LightHandle other)
{
  SetIndex(other.index_);
  return *this;
}

const glm::vec4& LightHandle::GetPosition() const
{
  return getData().position_;
}

glm::vec4 LightHandle::GetDirection() const
{
  return -getData().direction_();
}

const glm::vec3& LightHandle::GetDiffuseColor() const
{
  return getData().diffuseColor_;
}

const glm::vec3& LightHandle::GetSpecularColor() const
{
  return getData().specularColor_;
}

const glm::vec3& LightHandle::GetAmbientColor() const
{
  return getData().ambientColor_;
}

Light::LightType LightHandle::GetType() const
{
  return getData().type_;
}

std::pair<float, float> LightHandle::GetSpotlightAngles() const
{
  const Light& data = getData();
  return std::make_pair(data.spotlightInnerAngle_, data.spotlightOuterAngle_);
}

float LightHandle::GetSpotlightFalloff() const
{
  return getData().spotlightFalloff_;
}

bool LightHandle::IsActive() const
{
  return getData().active_;
}

size_t LightHandle::GetIndex() const
{
  return index_;
}

void LightHandle::SetPosition(const glm::vec3& pos)
{
  SetPosition(glm::vec4(pos, 1));
}

// ReSharper disable CppMemberFunctionMayBeConst
void LightHandle::SetPosition(const glm::vec4& pos)
{
  Light& data = getData();
  glm::vec4& currentPos = data.position_;
  if(currentPos != pos)
  {
    currentPos = pos;
    if(data.active_) LightSystem::SetDirty(index_);
  }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LightHandle::SetDirection(const glm::vec4& dir)
{
  Light& data = getData();
  glm::vec4& currentDir = data.direction_;
  if (currentDir != -dir)
  {
    currentDir = -dir;
    if (data.active_) LightSystem::SetDirty(index_);
  }
}

// ReSharper disable CppMemberFunctionMayBeConst
void LightHandle::SetDiffuseColor(const glm::vec3& color)
{
  Light& data = getData();
  glm::vec3& currentColor = data.diffuseColor_;
  if (currentColor != color)
  {
    currentColor = color;
    if (data.active_) LightSystem::SetDirty(index_);
  }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LightHandle::SetSpecularColor(const glm::vec3& color)
{
  Light& data = getData();
  glm::vec3& currentColor = data.specularColor_;
  if (currentColor != color)
  {
    currentColor = color;
    if (data.active_) LightSystem::SetDirty(index_);
  }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LightHandle::SetAmbientColor(const glm::vec3& color)
{
  Light& data = getData();
  glm::vec3& currentColor = data.ambientColor_;
  if (currentColor != color)
  {
    currentColor = color;
    if (data.active_) LightSystem::SetDirty(index_);
  }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LightHandle::SetType(LightType type)
{
  Light& data = getData();
  LightType& currentType = data.type_;
  if (currentType != type)
  {
    currentType = type;
    if (data.active_) LightSystem::SetDirty(index_);
  }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LightHandle::SetSpotlightAngles(float inner, float outer)
{
  Light& data = getData();
  float& currentInner = data.spotlightInnerAngle_;
  if (currentInner != inner)
  {
    currentInner = inner;
    if (data.active_) LightSystem::SetDirty(index_);
  }

  float& currentOuter = data.spotlightOuterAngle_;
  if (currentOuter != outer)
  {
    currentOuter = outer;
    if (data.active_) LightSystem::SetDirty(index_);
  }
}

void LightHandle::SetSpotlightAngles(const std::pair<float, float>& angles)
{
  SetSpotlightAngles(angles.first, angles.second);
}

void LightHandle::SetSpotlightFalloff(float falloff)
{
  Light& data = getData();
  float& currentFalloff = data.spotlightFalloff_;
  if (currentFalloff != falloff)
  {
    currentFalloff = falloff;
    if (data.active_) LightSystem::SetDirty(index_);
  }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LightHandle::SetActive(bool active)
{
  bool &currentActive = getData().active_;
  if(currentActive != active)
  {
    currentActive = active;
    LightSystem::SetDirty(index_);
  }
}

void LightHandle::Activate()
{
  SetActive(true);
}

void LightHandle::Deactivate()
{
  SetActive(false);
}

void LightHandle::SetIndex(size_t index)
{
  index_ = index;
}

void LightHandle::Clone(const LightHandle other)
{
  SetPosition(other.GetPosition());

  SetDiffuseColor(other.GetDiffuseColor());
  SetSpecularColor(other.GetSpecularColor());
  SetAmbientColor(other.GetAmbientColor());

  SetType(other.GetType());
  SetSpotlightAngles(other.GetSpotlightAngles());

  SetActive(other.IsActive());
}

Light& LightHandle::getData() const
{
  return LightSystem::GetRawLightData(index_);
}
