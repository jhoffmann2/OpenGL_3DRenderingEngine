#include "LightSystem.h"

#include <GL/glew.h>



LightSystem& LightSystem::Instance()
{
  static LightSystem system;
  return system;
}

void LightSystem::Init()
{
  LightSystem& instance = Instance();
  glGenBuffers(1, &instance.ssbo_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, instance.ssbo_);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shaderData_), &instance.shaderData_, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ShaderData::binding_, instance.ssbo_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightSystem::Update()
{
  LightSystem& instance = Instance();
  if(instance.dirtyLights_.any())
  {
    for (size_t i = 0; i < instance.dirtyLights_.size(); ++i)
    {
      if (instance.dirtyLights_[i])
      {
        const GLbyte* data = reinterpret_cast<GLbyte*>(&GetRawLightData(i));
        const GLbyte* begin = reinterpret_cast<GLbyte*>(&instance.shaderData_);
        const size_t offset = data - begin;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instance.ssbo_);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Light), data);
      }
    }

    // unbind buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    instance.dirtyLights_ = {false}; // clear dirty lights
  }
  if(instance.dirtyGlobalProps_)
  {
    // copy global properties over to the gpu
    const GLbyte* data = reinterpret_cast<GLbyte*>(&instance.shaderData_.globalProperties_);
    const GLbyte* begin = reinterpret_cast<GLbyte*>(&instance.shaderData_);
    const size_t offset = data - begin;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, instance.ssbo_);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(ShaderData::GlobalProperties), data);
    instance.dirtyGlobalProps_ = false;
  }
}

void LightSystem::SetDirty(const Light& light)
{
  Instance().dirtyLights_[LightIndex(light)] = true;
}

void LightSystem::SetDirty(size_t index)
{
  Instance().dirtyLights_[index] = true;
}

void LightSystem::SetLightAttenuation(const glm::vec3& attenuation)
{
  LightSystem& instance = Instance();
  instance.shaderData_.globalProperties_.lightAttenuation_ = attenuation;
  instance.dirtyGlobalProps_ = true;
}

void LightSystem::SetFog(const glm::vec3& color, float near, float far)
{
  LightSystem& instance = Instance();
  instance.shaderData_.globalProperties_.fogColor_ = color;
  instance.shaderData_.globalProperties_.fogNear_ = near;
  instance.shaderData_.globalProperties_.fogFar_ = far;
  instance.dirtyGlobalProps_ = true;
}

void LightSystem::SetAmbientColor(const glm::vec3& color)
{
  LightSystem& instance = Instance();
  instance.shaderData_.globalProperties_.ambientColor_ = color;
  instance.dirtyGlobalProps_ = true;
}

void LightSystem::SetEyePos(const glm::vec3& pos)
{
  LightSystem& instance = Instance();
  instance.shaderData_.globalProperties_.eyePos_ = pos;
  instance.dirtyGlobalProps_ = true;
}

void LightSystem::SetExposure(float strength)
{
    LightSystem& instance = Instance();
    instance.shaderData_.globalProperties_.exposure_ = strength;
    instance.dirtyGlobalProps_ = true;
}

void LightSystem::SetContrast(float strength)
{
    LightSystem& instance = Instance();
    instance.shaderData_.globalProperties_.contrast_ = strength;
    instance.dirtyGlobalProps_ = true;
}

void LightSystem::SetSpecularSamplingLevel(int level)
{
    LightSystem& instance = Instance();
    level = glm::clamp(level, 0, (int)MaxSpecularSamplingLevel());
    instance.shaderData_.globalProperties_.specularSamplingLevel_ = level;
    instance.dirtyGlobalProps_ = true;
}

void LightSystem::SetAmbientOcclusionRadius(float radius)
{
    LightSystem& instance = Instance();
    radius = glm::max(0.f, radius);
    instance.shaderData_.globalProperties_.aoRadius_ = radius;
    instance.dirtyGlobalProps_ = true;
}

void LightSystem::SetAmbientOcclusionIntensity(float intensity)
{
    LightSystem& instance = Instance();
    intensity = glm::max(0.f, intensity);
    instance.shaderData_.globalProperties_.aoIntensity_ = intensity;
    instance.dirtyGlobalProps_ = true;
}

void LightSystem::SetAmbientOcclusionContrast(float contrast)
{
    LightSystem& instance = Instance();
    contrast = glm::max(0.f, contrast);
    instance.shaderData_.globalProperties_.aoContrast_ = contrast;
    instance.dirtyGlobalProps_ = true;
}

const glm::vec3& LightSystem::GetLightAttenuation()
{
  return Instance().shaderData_.globalProperties_.lightAttenuation_;
}

const glm::vec3& LightSystem::GetFogColor()
{
  return Instance().shaderData_.globalProperties_.fogColor_;
}

std::pair<float, float> LightSystem::GetFogRange()
{
  return std::make_pair(
    Instance().shaderData_.globalProperties_.fogNear_(),
    Instance().shaderData_.globalProperties_.fogFar_()
  );
}

const glm::vec3& LightSystem::GetAmbientColor()
{
  return Instance().shaderData_.globalProperties_.ambientColor_;
}

const glm::vec3& LightSystem::GetEyePos()
{
  return Instance().shaderData_.globalProperties_.eyePos_;
}

float LightSystem::GetExposure()
{
    return Instance().shaderData_.globalProperties_.exposure_;
}

float LightSystem::GetContrast()
{
    return Instance().shaderData_.globalProperties_.contrast_;
}

Light& LightSystem::GetRawLightData(size_t index)
{
  return Instance().shaderData_.lights_[index];
}

size_t LightSystem::LightIndex(const Light& light)
{
  // use the pointer offset to calculate the index of the light
  return reinterpret_cast<const AlignData<16, Light>*>(&light) - &Instance().shaderData_.lights_[0];
}

int LightSystem::GetSpecularSamplingLevel()
{
    return Instance().shaderData_.globalProperties_.specularSamplingLevel_;
}

size_t LightSystem::MaxSpecularSamplingLevel()
{
    return Instance().shaderData_.globalProperties_.hammersley.size();
}

float LightSystem::GetAmbientOcclusionRadius()
{
    return Instance().shaderData_.globalProperties_.aoRadius_;
}

float LightSystem::GetAmbientOcclusionIntensity()
{
    return Instance().shaderData_.globalProperties_.aoIntensity_;
}

float LightSystem::GetAmbientOcclusionContrast()
{
    return Instance().shaderData_.globalProperties_.aoContrast_;
}
