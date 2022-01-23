#pragma once
#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include "Utilities.h"


class VertexGlobalSystem
{
public:
  static void Init();
  static void SetCamToNDC(const glm::mat4 &mat);
  static void SetWorldToCam(const glm::mat4 &mat);
  static void SetModelToWorld(const glm::mat4 &mat);

  [[nodiscard]] static const glm::mat4& GetCamToNDC();
  [[nodiscard]] static const glm::mat4& GetWorldToCam();
  [[nodiscard]] static const glm::mat4& GetModelToWorld();
  [[nodiscard]] static const glm::mat4& GetModelNToWorldN();
private:
  static VertexGlobalSystem& Instance();

  template<typename T>
  static void WriteToGPU(const T *element)
  {
    VertexGlobalSystem& instance = Instance();
    const GLbyte* data = reinterpret_cast<const GLbyte*>(element);
    const GLbyte* begin = reinterpret_cast<const GLbyte*>(&instance.shaderData_);
    const size_t offset = data - begin;
    glBindBuffer(GL_UNIFORM_BUFFER, instance.uboBlock_);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), data);
  }

  static void WriteToGPU(const void *begin_void, size_t size)
  {
    VertexGlobalSystem& instance = Instance();
    const GLbyte* data_begin = static_cast<const GLbyte*>(begin_void);
    const GLbyte* buffer_begin = reinterpret_cast<const GLbyte*>(&instance.shaderData_);
    const size_t offset = data_begin - buffer_begin;
    glBindBuffer(GL_UNIFORM_BUFFER, instance.uboBlock_);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data_begin);
  }

  VertexGlobalSystem();

  // force 16 byte alignment between arrays
  __declspec(align(16)) struct ShaderData
  {
    AlignData<64, glm::mat4> camToNDC_ = glm::mat4{1};
    AlignData<64, glm::mat4> worldToCam_ = glm::mat4{1};
    AlignData<64, glm::mat4> modelToWorld_ = glm::mat4{1};
    AlignData<64, glm::mat4> modelNToWorldN_ = glm::mat4{1};
    static constexpr GLuint binding_ = 2;
  } shaderData_;
  GLuint uboBlock_ = 0;
};

