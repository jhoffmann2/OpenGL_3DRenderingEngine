#include "DebugDraw.h"
#include "ntg/simplex.inl"
#include "Utilities.h"
#include <string>
#include "shader.hpp"

void DebugDraw::SetColor(const glm::vec3& color)
{
  DebugDraw& instance = Instance();
  glUseProgram(instance.shader);
  glUniform4fv(Instance().ucolor, 1, glm::data(color));
}

void DebugDraw::DrawWireframe(const std::vector<ntg::triangle3>& triangles)
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

  glLineWidth(10.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
  shader = LoadShaders("../../Common/shaders/Forward/LineRender.vert",
    "../../Common/shaders/Forward/LineRender.frag");
  ucolor = glGetUniformLocation(shader, "color");
}
