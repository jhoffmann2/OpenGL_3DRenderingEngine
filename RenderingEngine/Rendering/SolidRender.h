/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongRender.h
Purpose: 3D lighting renderer
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#ifndef CS250_PHONGRENDER_H
#define CS250_PHONGRENDER_H

#include "Materials/Texture.h"
#include "Mesh/Mesh.h"
#include <GL/gl.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Texture;
class Camera;

class SolidRender {
public:
  enum SHADER
  {
    PHONG_LIGHTING,
    PHONG_SHADING,
    BLINN_LIGHTING,
    BLINN_SHADING,
    FLAT_EMISSION,
    DEFFERED,
    LOCAL_LIGHT,
    DEPTH_MAP,

    SHADER_PROGRAM_COUNT
  };
  static SolidRender& Instance();
  static void clear(const glm::vec4 &color);
  static int loadMesh(const Mesh &m);
  static void unloadMesh(int mi);
  static void draw(int mi, Texture &diffuseTex, Texture &specularTex);
  static void drawDeferredSolid(int mi);
  static SHADER GetShader();
  static void SetShader(SHADER shadertype);
  static void ReloadShaders();
private:
  void LoadShaders();
  static GLuint GetProgram();
  SolidRender(void);
  ~SolidRender(void);
  struct MeshData {
    enum { VERT, NORM, UV, V_MAT, FACE, VERTEX_ATTRIB_COUNT };
    GLuint vertexArrayBuffer_,
           bufferObjects_[VERTEX_ATTRIB_COUNT];
    size_t faceCount_;
  };

  GLint udiffuseTex, uSpecularTex;

  GLuint programs_[SHADER_PROGRAM_COUNT] = {0};
  SHADER program_;
  std::vector<MeshData> meshData_;
};


#endif

