/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: functionality for constructing meshes out of obj files or generating sphere meshes
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#pragma once
#include "Utilities/Utilities.h"
#include "ntg/bounds.h"
#include <assimp/scene.h>
#include <vector>

struct Mesh
{
  std::vector<glm::vec3> vertices;
  std::vector<glm::uvec3> faces;
  std::vector<glm::vec3> vertex_normals;
  std::vector<glm::vec2> vertex_uv;
  std::vector<glm::vec3> face_normals;
  std::vector<int> vertex_material;
};
enum UV_MAPPING
{
  UV_NONE,
  UV_PLANAR,
  UV_CYLINDRICAL,
  UV_SPHERICAL
};

Mesh ObjToMesh(const std::string& path, UV_MAPPING uvMapping = UV_NONE);
Mesh generateSphereMesh(size_t division_count, UV_MAPPING uvMapping = UV_NONE);
Mesh aiSceneToMesh(const aiScene* scene);
Mesh aiImportMesh(const std::string& path);

ntg::bounds3 meshBounds(const Mesh& mesh);
ntg::bounds3 meshBounds(const std::vector<Mesh>& meshes);

void meshBoundsExpand(const std::vector<Mesh>& meshes, glm::vec3 &min, glm::vec3 &max);
glm::mat4 CenterMeshTransform(const std::vector<Mesh> &meshes);
glm::mat4 CenterMeshTransform(const glm::vec3& min, const glm::vec3& max);
glm::mat4 CenterMeshTransform(const ntg::bounds3& bounds);
void MeshTransform(std::vector<Mesh> &meshes, const glm::mat4 &transform);

void ImportMeshList(
  const std::string& path, 
  const std::string& file,
  std::vector<Mesh>& meshes,
  std::vector<std::string>& names
);
