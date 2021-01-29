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

void LightSystem::SetEyePos(const glm::vec4& pos)
{
  LightSystem& instance = Instance();
  instance.shaderData_.globalProperties_.eyePos_ = pos;
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

const glm::vec4& LightSystem::GetEyePos()
{
  return Instance().shaderData_.globalProperties_.eyePos_;
}

Light& LightSystem::GetRawLightData(int index)
{
  return Instance().shaderData_.lights_[index];
}

size_t LightSystem::LightIndex(const Light& light)
{
  // use the pointer offset to calculate the index of the light
  return reinterpret_cast<const AlignData<16, Light>*>(&light) - &Instance().shaderData_.lights_[0];
}
