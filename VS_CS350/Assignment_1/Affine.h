/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Affine.h
Purpose: basic math for affine transformations
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#ifndef CS250_AFFINE_H
#define CS250_AFFINE_H


#include <cmath>
#include <glm/glm.hpp>


  inline
  bool near(float x, float y) { return std::abs(x-y) < 1e-4f; }

  glm::vec4 point(float x, float y, float z);
  glm::vec4 vector(float x, float y, float z);
  bool isPoint(const glm::vec4 &v);
  bool isVector(const glm::vec4 &v);

  float angle(const glm::vec4 &u, const glm::vec4 &v);
  glm::vec4 cross(const glm::vec4 &u, const glm::vec4 &v);

  glm::mat4 affine(const glm::vec4 &Lx, const glm::vec4 &Ly,
                   const glm::vec4 &Lz, const glm::vec4 &C);
  bool isAffine(const glm::mat4 &M);

  glm::mat4 rotate(float deg, const glm::vec4 &v);
  glm::mat4 scale(float r);
  glm::mat4 scale(float rx, float ry, float rz);
  glm::mat4 scale(const glm::vec4& v);
  glm::mat4 translate(const glm::vec4 &v);
  glm::mat4 affineInverse(const glm::mat4 &A);



#endif

