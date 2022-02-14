#pragma once
#include <GL/glew.h>
#include <camera/Camera.h>
#include <glm/mat4x4.hpp>
#include <ntg/bounds.h>

#include "Utilities/Utilities.h"

class ShaderGlobalSystem
{
public:
  static void Init();
  static void SetCamToNDC(const glm::mat4 &mat);
  static void SetCamToWorld(const glm::mat4 &mat);
  static void SetModelToWorld(const glm::mat4 &mat);
  static void SetShadowWorldToNDC(const glm::mat4 &mat);
  static void SetupForShadowMap(const Camera& cam, const ntg::bounds3 &world_bounds);

  [[nodiscard]] static const glm::mat4& GetCamToNDC();
  [[nodiscard]] static const glm::mat4& GetWorldToCam();
  [[nodiscard]] static const glm::mat4& GetModelToWorld();
  [[nodiscard]] static const glm::mat4& GetModelNToWorldN();
private:
  static ShaderGlobalSystem & Instance();

  template<typename T>
  static void WriteToGPU(const T *element)
  {
    ShaderGlobalSystem & instance = Instance();
    const GLbyte* data = reinterpret_cast<const GLbyte*>(element);
    const GLbyte* begin = reinterpret_cast<const GLbyte*>(&instance.shaderData_);
    const size_t offset = data - begin;
    glBindBuffer(GL_UNIFORM_BUFFER, instance.uboBlock_);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), data);
  }

  static void WriteToGPU(const void *begin_void, size_t size)
  {
    ShaderGlobalSystem & instance = Instance();
    const GLbyte* data_begin = static_cast<const GLbyte*>(begin_void);
    const GLbyte* buffer_begin = reinterpret_cast<const GLbyte*>(&instance.shaderData_);
    const size_t offset = data_begin - buffer_begin;
    glBindBuffer(GL_UNIFORM_BUFFER, instance.uboBlock_);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data_begin);
  }

  ShaderGlobalSystem();

  // force 16 byte alignment between arrays
  __declspec(align(16)) struct ShaderData
  {
    AlignData<64, glm::mat4> camToNDC_ = glm::mat4{1};
    AlignData<64, glm::mat4> worldToCam_ = glm::mat4{1};
    AlignData<64, glm::mat4> camToWorld_ = glm::mat4{1};
    AlignData<64, glm::mat4> modelToWorld_ = glm::mat4{1};
    AlignData<64, glm::mat4> modelNToWorldN_ = glm::mat4{1};
    AlignData<64, glm::mat4> shadowWorldToNDC_ = glm::mat4{1};
    AlignData<16, glm::vec2> depth_range = glm::vec2{0.f, 1.f};
    static constexpr GLuint binding_ = 2;
  } shaderData_;
  GLuint uboBlock_ = 0;
};

