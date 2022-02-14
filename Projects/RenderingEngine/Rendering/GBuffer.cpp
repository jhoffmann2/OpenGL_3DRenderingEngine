#include "GBuffer.h"

#include <cassert>
#include <imgui/imgui.h>
#include <numeric>

#include "SolidRender.h"
#include "shader.hpp"

void GBuffer::Init(size_t width, size_t height) {
  GBuffer &instance = Instance();
  instance.width = width;
  instance.height = height;

  GLuint &gBufferFBO = instance.gBufferFBO;
  GLuint &depthTexture = instance.depthTexture;
  GLuint &rboDepth = instance.rboDepth;

  std::array<GLuint, RenderTargetCount> &textures = instance.textures;
  std::array<GLenum, RenderTargetCount> &drawBuffers = instance.drawBuffers;

  // create the FBO
  glGenFramebuffers(1, &gBufferFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

  // create the gbuffer textures
  glGenTextures(RenderTargetCount, textures.data());
  glGenTextures(1, &depthTexture);

  for (size_t i = 0; i < RenderTargetCount; ++i) {
    glBindTexture(GL_TEXTURE_2D, textures[i]);
    if (i == TARGET_SHADOW) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)height,
                   (GLsizei)height, 0, GL_RGBA, GL_FLOAT, nullptr);
    } else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)width,
                   (GLsizei)height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)i,
                           GL_TEXTURE_2D, textures[i], 0);
  }

  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (GLsizei)width,
                        (GLsizei)height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth);

  std::iota(drawBuffers.begin(), drawBuffers.end(), GL_COLOR_ATTACHMENT0);
  glDrawBuffers(RenderTargetCount, drawBuffers.data());

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  instance.SetupFSQ();
}

void GBuffer::Bind() {
  GLuint &gBufferFBO = Instance().gBufferFBO;

  glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
}

void GBuffer::UnBind() {
  GBuffer &instance = Instance();

  if (Instance().copyDepth_) {

    GLuint &gBufferFBO = instance.gBufferFBO;
    size_t &width = instance.width;
    size_t &height = instance.height;

    // copy gbuffer depth info over to default FBO
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, (GLint)width, (GLint)height, 0, 0, (GLint)width,
                      (GLint)height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  }

  // bind default FBO to screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  for (size_t i = 0; i < RenderTargetCount; ++i) {
    GLint swizzle[] = {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA};
    glTextureParameteriv(instance.textures[i], GL_TEXTURE_SWIZZLE_RGBA,
                         swizzle);
  }
}

void GBuffer::RenderFSQ() {
  auto &instance = Instance();
  auto &FSQ = instance.FSQ;
  glUseProgram(FSQ.shader_program);

  // load textures
  for (int i = 0; i < RenderTargetCount; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, instance.textures[i]);
    glUniform1i(FSQ.uTex[i], i);
  }

  // references to inside of data to avoid rewriting code
  GLuint &vao = FSQ.vertexArrayBuffer_;
  int &face_count = FSQ.faceCount_;

  // turn off writing to depth buffer
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  // recall state
  glBindVertexArray(vao);
  glLineWidth(3.0f);
  glDrawElements(GL_TRIANGLES, 3 * face_count, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);

  // turn depth buffer back on
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}

void GBuffer::RenderSolid(GLuint &vao, size_t &face_count) {
  auto &instance = Instance();

  // load textures
  for (int i = 0; i < RenderTargetCount; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, instance.textures[i]);
    glUniform1i(instance.FSQ.uTex[i], i);
  }

  // turn off writing to depth buffer
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);
  glCullFace(GL_FRONT);
  glEnable(GL_CULL_FACE);

  // recall state
  glBindVertexArray(vao);
  glLineWidth(3.0f);
  glDrawElements(GL_TRIANGLES, 3 * face_count, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);

  glCullFace(GL_BACK);
  glDisable(GL_BLEND);

  // turn depth buffer back on
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}

void GBuffer::ImguiEditor() {
  if (ImGui::Begin("Deferred Shading")) {

    ImGui::Text("Normal Target");
    ImGuiImage(TARGET_NORMAL);

    ImGui::NewLine();
    static int worldormat = 0;
    ImGui::Text("WorldPosition and Material");
    ImGui::Combo("##WorldPosition and Material",
                 reinterpret_cast<int *>(&worldormat),
                 "WORLD_POSITION (RGB channels)\0"
                 "MATERIAL (Alpha channel)\0");
    ImGuiImage(TARGET_WORLD_POS, worldormat);

    ImGui::NewLine();
    static int difforspec = 0;
    ImGui::Text("Diffuse and Specular Textures");
    ImGui::Combo("##Diffuse and Specular Textures",
                 reinterpret_cast<int *>(&difforspec),
                 "DIFFUSE (RGB channels)\0"
                 "SPECULAR (Alpha channel)\0");
    ImGuiImage(TARGET_DIFFUSE, difforspec);
    ImGui::TextWrapped("^^ Note that im not using any textures right now so "
                       "all white is correct output");

    ImGui::Text("Shadow Depth Target");
    ImGuiImage(TARGET_SHADOW);
  }
  ImGui::End();
}

void GBuffer::ImGuiImage(RenderTarget target, int swizzle_id) {
  auto &instance = Instance();
  const float width = ImGui::GetWindowContentRegionWidth();
  float aratio =
      static_cast<float>(instance.height) / static_cast<float>(instance.width);

  if (target == TARGET_SHADOW)
    aratio = 1.f;

  const GLuint texture = (target != DEPTH_TEXTURE) ? instance.textures[target]
                                                   : instance.depthTexture;

  switch (swizzle_id) {
  case 0:
    glTextureParameteri(texture, GL_TEXTURE_SWIZZLE_A, GL_ONE);
    break;
  case 1: {
    GLint swizzle[] = {GL_ALPHA, GL_ALPHA, GL_ALPHA, GL_ONE};
    glTextureParameteriv(texture, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    break;
  }
  default:
    assert("Illegal swizzle ID");
  }

  ImGui::Image((void *)(intptr_t)texture, {width, width * aratio}, {0, 1},
               {1, 0});
}

GBuffer &GBuffer::Instance() {
  static GBuffer buffer;
  return buffer;
}

void GBuffer::SetupFSQ() {
  FSQ.shader_program =
      LoadShaders("../../Common/shaders/Deferred/DeferredPhong.vert",
                  "../../Common/shaders/Deferred/DeferredPhong.frag");

  static const std::array<glm::vec3, 6> vertices = {{
      {-1.0f, -1.0f, 0.0f},
      {1.0f, -1.0f, 0.0f},
      {1.0f, 1.0f, 0.0f},
      {-1.0f, 1.0f, 0.0f},
  }};

  static const std::array<glm::vec2, 6> vertex_uv = {{
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
  }};

  static const std::array<glm::uvec3, 2> faces = {{{0, 1, 2}, {0, 2, 3}}};

  // references to inside of data to avoid rewriting code
  GLuint &vertex_buffer = FSQ.bufferObjects_[FSQ_DATA::VERT];
  GLuint &face_buffer = FSQ.bufferObjects_[FSQ_DATA::FACE];
  GLuint &uv_buffer = FSQ.bufferObjects_[FSQ_DATA::UV];
  GLuint &vao = FSQ.vertexArrayBuffer_;
  int &face_count = FSQ.faceCount_;

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &uv_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertex_uv.size(),
               vertex_uv.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &face_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * faces.size(),
               faces.data(), GL_STATIC_DRAW);

  // create vertex array object
  glGenVertexArrays(1, &vao);
  //   start state recording
  glBindVertexArray(vao);

  // assign postion attribute to vertex_buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  const GLint aposition = glGetAttribLocation(FSQ.shader_program, "position");
  glVertexAttribPointer(aposition, 3, GL_FLOAT, false, 0, nullptr);
  glEnableVertexAttribArray(aposition);

  // assign uv attribute to uv_buffer
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
  const GLint auv = glGetAttribLocation(FSQ.shader_program, "uv_vert");
  glVertexAttribPointer(auv, 2, GL_FLOAT, false, 0, nullptr);
  glEnableVertexAttribArray(auv);

  // ready the face buffer for drawing
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face_buffer);
  //   stop state recording
  glBindVertexArray(0);

  face_count = static_cast<int>(faces.size());

  FSQ.uTex[TARGET_DIFFUSE] =
      glGetUniformLocation(FSQ.shader_program, "diffuseTex");
  FSQ.uTex[TARGET_NORMAL] =
      glGetUniformLocation(FSQ.shader_program, "normalTex");
  FSQ.uTex[TARGET_WORLD_POS] =
      glGetUniformLocation(FSQ.shader_program, "worldPosTex");
  FSQ.uTex[TARGET_SHADOW] =
      glGetUniformLocation(FSQ.shader_program, "shadowMap");
}

void GBuffer::BlurTarget(GBuffer::RenderTarget target, GLuint blurRadius)
{
  if(blurRadius == 0)
    return;

  auto &instance = Instance();
  GLuint w = (target != TARGET_SHADOW) ? Instance().width : Instance().height;
  GLuint h = Instance().height;

  static std::array<GLuint, 2> gaussianBlurShaders = {
      LoadComputeShader("../../Common/shaders/Compute/GaussianBlurX.comp"),
      LoadComputeShader("../../Common/shaders/Compute/GaussianBlurY.comp")};
  static std::array<glm::uvec3, 2> dispatchDimensions {{
      {w / 128u, h, 1u},
      {w, h / 128u, 1u}
  }};

  glEnable(GL_DEBUG_OUTPUT);

  // Set all uniform and image variables
  static GLuint uboBlock = [programID = gaussianBlurShaders[0]]() -> GLuint {
    GLuint uboBlock;
    glGenBuffers(1, &uboBlock);
    GLuint bindpoint = 3;

    auto loc = glGetUniformBlockIndex(programID, "blurKernel");
    glUniformBlockBinding(programID, loc, bindpoint);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBlock);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindpoint, uboBlock);
    glBufferData(GL_UNIFORM_BUFFER, 16 * (2 * 100 + 1), nullptr, GL_STATIC_DRAW);

    return uboBlock;
  }();

  static std::vector<glm::vec4> blurKernal;
  if(blurKernal.size() != blurRadius * 2 + 1)
  {
    // recalculate blur kernal at new size
    blurKernal.resize(blurRadius * 2 + 1);
    float sum = 0;
    for(size_t i = 0; i < blurKernal.size(); ++i)
    {
      const float s = blurRadius / 3.f;
      const float frac = ((float)i-(float)blurRadius) / s;
      blurKernal[i].x = glm::exp(-0.5f * frac * frac);
      sum += blurKernal[i].x;
    }

    // normalize weights
    for(size_t i = 0; i < blurKernal.size(); ++i)
      blurKernal[i] /= sum;

    // upload new blur kernal to gpu
    glBindBuffer(GL_UNIFORM_BUFFER, uboBlock);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4) * blurKernal.size(), blurKernal.data());
  }

  for(size_t i = 0; i < gaussianBlurShaders.size(); ++i)
  {
    const GLuint programID = gaussianBlurShaders[i];
    const glm::uvec3& dim = dispatchDimensions[i];
    glUseProgram(programID);
    glUniform1ui(1, blurRadius);

    static GLuint srcTex = [w, h, target, &instance](){
      GLuint srcTex;
      glCreateTextures(GL_TEXTURE_2D, 1, &srcTex);
      glTextureStorage2D(srcTex, 1, GL_RGBA32F, w, h);
      return srcTex;
    }();

    glCopyImageSubData(
        instance.textures[target], GL_TEXTURE_2D, 0, 0, 0, 0,
        srcTex, GL_TEXTURE_2D, 0, 0, 0, 0,
        w, h, 1
    );


    GLuint imageUnit = 1;
    GLuint loc = glGetUniformLocation(programID, "src");
    glBindImageTexture(imageUnit, srcTex, 0, GL_FALSE, 0,
                       GL_READ_ONLY, GL_RGBA32F);
    glUniform1i(loc, imageUnit);

    imageUnit = 2;
    loc = glGetUniformLocation(programID, "dst");
    glBindImageTexture(imageUnit, instance.textures[target], 0, GL_FALSE, 0,
                       GL_READ_ONLY, GL_RGBA32F);
    glUniform1i(loc, imageUnit);

    glDispatchCompute(dim.x, dim.y, dim.z);
    glUseProgram(0);
  }
  glDisable(GL_DEBUG_OUTPUT);
}

void GBuffer::Clear()
{
  auto &instance = Instance();

  glm::vec4 clearColor(std::numeric_limits<float>::max());
  glClearTexSubImage(
      instance.textures[TARGET_DIFFUSE],
      0, // level
      0,0,0, // offset
      instance.width, instance.height, // size
      1, // depth
      GL_RGBA, GL_FLOAT, // format & type
      &clearColor
  );

  glClearTexSubImage(
      instance.textures[TARGET_WORLD_POS],
      0, // level
      0,0,0, // offset
      instance.width, instance.height, // size
      1, // depth
      GL_RGBA, GL_FLOAT, // format & type
      &clearColor
  );

  glClearTexSubImage(
      instance.textures[TARGET_NORMAL],
      0, // level
      0,0,0, // offset
      instance.width, instance.height, // size
      1, // depth
      GL_RGBA, GL_FLOAT, // format & type
      &clearColor
  );

  clearColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
  glClearTexSubImage(
      instance.textures[TARGET_SHADOW],
      0, // level
      0,0,0, // offset
      instance.height, instance.height, // size
      1, // depth
      GL_RGBA, GL_FLOAT, // format & type
      &clearColor
  );

  glClearDepth(1);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}
