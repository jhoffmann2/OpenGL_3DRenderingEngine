#pragma once
#include <vector>
#include <GL/glew.h>

#include "ntg/bounds.h"
#include "ntg/simplex.h"

class DebugDraw
{
public:
  DebugDraw(DebugDraw&) = delete;
  static void SetColor(const glm::vec4& color);
  static void DrawTriangleList(const std::vector<ntg::triangle3> &triangles, GLenum mode = GL_LINE);
  static void DrawLineList(const std::vector<ntg::lineseg3> &lines);
  static void DrawAABB(const ntg::bounds3& bounds, GLenum mode = GL_LINE);
private:
  static DebugDraw &Instance();
  DebugDraw();
  GLuint vbo, vao, shader;
  GLint ucolor;
};

