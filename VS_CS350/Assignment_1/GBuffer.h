#pragma once
#include <array>
#include <GL/glew.h>

#include "SolidRender.h"

class GBuffer
{
public:
  static void Init(size_t width, size_t height);
  static void Bind();
  static void UnBind();
  static void RenderFSQ();

  GBuffer(GBuffer &) = delete;
  GBuffer& operator=(GBuffer&) = delete;

private:
  enum RenderTarget
  {
    TARGET_DIFFUSE,
    TARGET_WORLD_POS,
    TARGET_NORMAL,

    RenderTargetCount
  };

  static GBuffer &Instance();

  void SetupFSQ();

  GBuffer() = default;
  ~GBuffer() = default;

  SolidRender::SHADER unbindShader_;

  GLuint gBufferFBO;
  GLuint depthTexture;
  GLuint rboDepth;
  std::array<GLuint, RenderTargetCount> textures;
  std::array<GLenum, RenderTargetCount> drawBuffers;

  size_t width, height;


  struct FSQ_DATA
  {
    GLuint shader_program;
    std::array<GLint, RenderTargetCount> uTex;

    enum { VERT = 0, FACE = 1, UV = 2 };
    GLuint vertexArrayBuffer_,
      bufferObjects_[3];
    int faceCount_;

  } FSQ;

  struct MeshData {
  };

};

