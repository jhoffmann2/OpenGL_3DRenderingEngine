#pragma once
#include <array>
#include <GL/glew.h>

#include "SolidRender.h"

class GBuffer
{
public:
  enum RenderTarget
  {
    TARGET_DIFFUSE,
    TARGET_WORLD_POS,
    TARGET_NORMAL,
    TARGET_SHADOW,
    TARGET_AMBIENT_OCCLUSION,

    RenderTargetCount,
    DEPTH_TEXTURE
  };

  static void Init(size_t width, size_t height);
  static void Bind();
  static void UnBind();
  static void RenderFSQ(const Texture& environmentTex, const Texture& irradianceTex);
  static void RenderSolid(GLuint &vao, size_t &face_count);
  static void ImguiEditor();

  static void BlurTarget(RenderTarget target, GLuint blurRadius);
  static void EdgeAwareBlurTarget(RenderTarget target, GLuint blurRadius);
  static void Clear();

  static void ReloadShaders();

  GBuffer(GBuffer &) = delete;
  GBuffer& operator=(GBuffer&) = delete;

private:
  static void ImGuiImage(RenderTarget target, int swizzle_id = 0);

  static GBuffer &Instance();

  void SetupFSQ();

  GBuffer() = default;
  ~GBuffer() = default;


  void RenderFSQ_Internal(const Texture& environmentTex, const Texture& irradianceTex);

  GLuint gBufferFBO;
  GLuint depthTexture;
  GLuint rboDepth;
  std::array<GLuint, RenderTargetCount> textures;
  std::array<GLenum, RenderTargetCount> drawBuffers;

  size_t width, height;
  bool copyDepth_ = true;


  struct FSQ_DATA
  {
    GLuint deferred_shader;
    GLuint ambient_occlusion_shader;
    std::array<GLint, RenderTargetCount> uTex;

    enum { VERT = 0, FACE = 1, UV = 2 };
    GLuint vertexArrayBuffer_,
      bufferObjects_[3];
    int faceCount_;

  } FSQ;

};

