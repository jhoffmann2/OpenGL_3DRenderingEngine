/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: VertexNormalRender.h
Purpose: renderer for displaying the normals of a meshes verticies
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/
#pragma once

#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "Mesh.h"


class Camera;

class VertexNormalRender {
public:
  static VertexNormalRender& Instance();
  static void clear(const glm::vec4 &color);
  static int loadMesh(const Mesh &m);
  static void unloadMesh(int mi);
  static void setModel(const glm::mat4 &M);
  static void setCamera(const Camera &cam);
  static void setRGB(const glm::vec4& color);
  static void draw(int mi);
private:
  VertexNormalRender(void);
  ~VertexNormalRender(void);
  GLint program;
  GLint upersp_matrix;
  GLint uview_matrix;
  GLint umodel_matrix;
  GLint ucolor;
  struct MeshData {
    enum { VERT, LINE };
    GLuint vertex_array_buffer;
    GLuint buffer_objects[2];
    int line_count;
  };
  std::vector<MeshData> mesh_data;
};

