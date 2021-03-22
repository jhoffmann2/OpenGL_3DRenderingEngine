#pragma once
#include <vector>
#include <GL/glew.h>
#include "ntg/simplex.h"

class DebugDraw
{
public:
  DebugDraw(DebugDraw&) = delete;
  static void SetColor(const glm::vec3& color);
  static void DrawWireframe(const std::vector<ntg::triangle3> &triangles);
private:
  static DebugDraw &Instance();
  DebugDraw();
  GLuint vbo, vao, shader;
  GLint ucolor;
};

