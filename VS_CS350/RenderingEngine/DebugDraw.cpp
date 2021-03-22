#include "DebugDraw.h"
#include "ntg/simplex.inl"
#include "Utilities.h"
#include <string>
#include "shader.hpp"

void DebugDraw::SetColor(const glm::vec4& color)
{
  DebugDraw& instance = Instance();
  glUseProgram(instance.shader);
  glUniform4fv(Instance().ucolor, 1, glm::data(color));
}

void DebugDraw::DrawTriangleList(const std::vector<ntg::triangle3>& triangles, GLenum mode)
{
  DebugDraw &instance = Instance();
  glUseProgram(instance.shader);
  glBufferData(
    GL_ARRAY_BUFFER, 
    triangles.size() * sizeof(ntg::triangle3),
    triangles.data(),
    GL_STREAM_DRAW
  );

  glBindVertexArray(instance.vao);

  glBindBuffer(GL_ARRAY_BUFFER, instance.vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
  glEnableVertexAttribArray(0);

  glLineWidth(5.0f);
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(triangles.size()) * 3);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void DebugDraw::DrawLineList(const std::vector<ntg::lineseg3>& lines)
{
  DebugDraw &instance = Instance();
  glUseProgram(instance.shader);
  glBufferData(
    GL_ARRAY_BUFFER, 
    lines.size() * sizeof(ntg::lineseg3),
    lines.data(),
    GL_STREAM_DRAW
  );

  glBindVertexArray(instance.vao);

  glBindBuffer(GL_ARRAY_BUFFER, instance.vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
  glEnableVertexAttribArray(0);

  glLineWidth(5.0f);
  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines.size()) * 2);
}

// yes im aware this function is discusting but since it's for debug
// i can't be bothered to make it more efficient
void DebugDraw::DrawAABB(const ntg::bounds3& bounds, GLenum mode)
{
  const glm::mat4x4 t = scale(translate(glm::mat4x4{ 1 }, bounds.center()), bounds.size());
  //const glm::mat4x4 t = translate(scale(glm::mat4x4{1}, bounds.size()), bounds.center());
  const glm::vec3 v[8] = {
    t * glm::vec4{.5f, .5f, -.5f, 1.f},
    t * glm::vec4{.5f, -.5f, -.5f, 1.f},
    t * glm::vec4{-.5f, -.5f, -.5f, 1.f},
    t * glm::vec4{-.5f, .5f, -.5f, 1.f},
    t * glm::vec4{.5f, .5f, .5f, 1.f},
    t * glm::vec4{.5f, -.5f, .5f, 1.f},
    t * glm::vec4{-.5f, -.5f, .5f, 1.f},
    t * glm::vec4{-.5f, .5f, .5f, 1.f},
  };
  glDisable(GL_CULL_FACE);
  glDepthMask(false);
  DrawLineList({
    {v[0], v[4]},
    {v[1], v[5]},
    {v[2], v[6]},
    {v[3], v[7]},

    {v[0], v[3]},
    {v[1], v[2]},
    {v[4], v[7]},
    {v[5], v[6]},

    {v[0], v[1]},
    {v[2], v[3]},
    {v[4], v[5]},
    {v[6], v[7]},
  });
  glDepthMask(true);
  glEnable(GL_CULL_FACE);
}

DebugDraw& DebugDraw::Instance()
{
  static DebugDraw instance;
  return instance;
}

DebugDraw::DebugDraw()
{
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  shader = LoadShaders("../../Common/shaders/Forward/DebugDraw.vert",
    "../../Common/shaders/Forward/DebugDraw.frag");
  ucolor = 0;
}
