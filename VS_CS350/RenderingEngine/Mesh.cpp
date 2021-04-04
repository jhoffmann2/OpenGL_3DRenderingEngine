/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.cpp
Purpose: functionality for constructing meshes out of obj files or generating sphere meshes
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#include "Mesh.h"

#include <algorithm>
#include <string>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtx/vector_angle.hpp>
#include "ntg/bounds.inl"


#include "Utilities.h"

#define LOG_ACTIVE false


#define M (2*division_count)
#define N (division_count)
#define NORTH (M*(N-1))
#define SOUTH (M*(N-1)+1)
#define PI 3.1415926536f

namespace
{
  void CenterMesh(std::vector<glm::vec3>& verticies)
  {
    auto minCorner = glm::vec3(std::numeric_limits<float>::infinity());
    auto maxCorner = glm::vec3(-std::numeric_limits<float>::infinity());
    for (const glm::vec3& v : verticies)
    {
      for (int i = 0; i < 3; ++i)
      {
        if (v[i] < minCorner[i]) minCorner[i] = v[i];
        if (v[i] > maxCorner[i]) maxCorner[i] = v[i];
      }
    }

    glm::vec3 bb_scale = maxCorner - minCorner;
    float max_distance = *std::max_element(glm::data(bb_scale), glm::data(bb_scale) + 3);

    glm::mat4 transform =
      glm::scale(1.f / max_distance) *
      glm::translate(-(maxCorner + minCorner) / 2.f);

    for (glm::vec3& v : verticies)
      v = transform * glm::vec4(v, 1);
  }

  glm::vec3 FaceNormal(const glm::uvec3 &face, const std::vector<glm::vec3> &verticies)
  {
    return normalize(cross(
      verticies[face[1]].xyz - verticies[face[0]].xyz,
      verticies[face[2]].xyz - verticies[face[0]].xyz
    ));
  }

  glm::vec2 planarTextureMap(glm::vec3 pos)
  {
    return pos.xy;
  }

  glm::vec2 cylindricalTextureMap(glm::vec3 pos)
  {
    float theta = glm::degrees(glm::atan(pos.z, pos.x));
    return glm::vec2(theta / 360, pos.y);
  }

  glm::vec2 sphericalTextureMap(glm::vec3 pos)
  {
    float theta = glm::degrees(glm::atan(pos.z, pos.x));
    float r = length(pos);//sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
    float phi = glm::degrees(acos(pos.y / r));

    return glm::vec2(theta / 360, phi / 180);
  }

  glm::vec2 calculateUV(glm::vec3 vtx_pos, UV_MAPPING mapping)
  {
    switch (mapping)
    {
    case UV_NONE: return {0,0};
    case UV_PLANAR: return planarTextureMap(vtx_pos);
    case UV_CYLINDRICAL: return cylindricalTextureMap(vtx_pos);
    case UV_SPHERICAL: return sphericalTextureMap(vtx_pos);
    }
    return { 0,0 };
  }
}


Mesh ObjToMesh(const std::string& path, UV_MAPPING uvMapping)
{
  std::ifstream file(path);
  Mesh mesh;
  auto& vertices = mesh.vertices;
  auto& faces = mesh.faces;
  auto& vertex_normals = mesh.vertex_normals;
  auto& face_normals = mesh.face_normals;
  auto& vertex_uv = mesh.vertex_uv;

  std::string line;
  while (std::getline(file, line))
  {

    size_t f = line.find('\\');
    std::string::iterator end = (f == std::string::npos) ? line.end() : (line.begin() + f);

    if (end - line.begin() < 3) continue; // skip empty lines

    std::stringstream dataStr(std::string(line.begin() + 2, end));
    switch (line[0])
    {
    case '#':
    {
      LOG << "discarding comment: " << dataStr.str() << std::endl;
      break;
    }
    case 'v':
    {
      if (line[1] == 't' || line[1] == 'n') break; // skip vertex textures and normals

      mesh.vertices.emplace_back();
      dataStr >> vertices.back();
      LOG << "created vertex " << vertices.size() - 1 << ": " << vertices.back() << std::endl;
      break;
    }
    case 'f':
    {
read_new_face:
      mesh.faces.emplace_back();
      mesh.face_normals.emplace_back();
      glm::uvec3& face = faces.back();
      glm::vec3& normal = face_normals.back();
      int slash_count = 0;
      for (char c : line)
        slash_count += (c == '/');
      if(slash_count == 0)
        dataStr >> face;
      else
      {
        for (glm::length_t i = 0; i < face.length(); ++i)
        {
          dataStr >> face[i];
          dataStr.ignore(100, ' ');// ignore index data for texture and normal data
        }
      }
      face -= glm::uvec3{ 1, 1, 1 }; // change 1 indexing to zero indexing
      normal = FaceNormal(face, vertices);

      if(!dataStr.eof())
      {
        // push first and last index of face back to the front of the stream
        char remainder[512] = {0};
        dataStr.getline(remainder, 512);
        dataStr = std::stringstream("");
        dataStr << face.x+1 << ' ';
        dataStr << face.z+1 << ' ';
        dataStr << remainder;
        goto read_new_face;
      }

      LOG << "created face " << faces.size() - 1 << ": " << face;
      LOG << " with normal: " << normal << std::endl;
      break;
    }
    default:
      LOG << "ERROR : obj line \"" << line << "\" not supported" << std::endl;
      //assert(false);
    }
  }

  vertex_normals.resize(vertices.size());
  for (size_t i = 0; i < faces.size(); ++i)
  {
    const glm::uvec3& face = faces[i];
    const glm::vec3& face_normal = face_normals[i];
    for (glm::length_t j = 0; j < 3; ++j)
    {
      const glm::vec3 &current = vertices[face[j]];
      const glm::vec3 &next = vertices[face[(j + 1) % 3]];
      const glm::vec3 &prev = vertices[face[(j + 2) % 3]];

      // weight the average towards larger angles
      float weight = glm::angle(next - current, prev - current) / 360.f;
      vertex_normals[face[j]] += weight * face_normal;
    }
  }

  CenterMesh(mesh.vertices);
  vertex_uv.reserve(vertices.size());
  for (const glm::vec3 pos : vertices)
  {
    vertex_uv.emplace_back(calculateUV(pos, uvMapping));
  }

  mesh.vertex_material.resize(vertices.size(), -1);

  return mesh;
}

Mesh generateSphereMesh(size_t division_count, UV_MAPPING uvMapping)
{
  Mesh mesh;
  auto& vertices = mesh.vertices;
  auto& normals = mesh.vertex_normals;
  auto& faces = mesh.faces;
  auto& fnormals = mesh.face_normals;
  auto& vertex_uv = mesh.vertex_uv;

  vertices.resize(M * (N - 1) + 2);
  normals.resize(M * (N - 1) + 2);

  for (size_t i = 1; i < N; ++i) {
    float theta = PI * i / N;
    for (size_t j = 0; j < M; ++j) {
      size_t index = M * (i - 1) + j;
      float phi = 2 * PI * j / M;
      normals[index].x = sin(theta) * cos(phi);
      normals[index].y = sin(theta) * sin(phi);
      normals[index].z = cos(theta);
    }
  }
  normals[NORTH] = glm::vec3(0.0f, 0.0f, 1.0f);
  normals[SOUTH] = glm::vec3(0.0f, 0.0f, -1.0f);

  for (unsigned n = 0; n < normals.size(); ++n) {
    vertices[n] = normals[n];
  }

  for (size_t i = 2; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      glm::uvec3 face;
      size_t jp1 = (j + 1) % M;
      face[0] = static_cast<unsigned>((M * (i - 2) + j));
      face[1] = static_cast<unsigned>(M * (i - 1) + jp1);
      face[2] = static_cast<unsigned>(M * (i - 2) + jp1);
      faces.push_back(face);
      fnormals.push_back(FaceNormal(face, vertices));
      face[1] = static_cast<unsigned>(M * (i - 1) + j);
      face[2] = static_cast<unsigned>(M * (i - 1) + jp1);
      faces.push_back(face);
      fnormals.push_back(FaceNormal(face, vertices));
    }
  }

  for (size_t j = 0; j < M; ++j) {
    glm::uvec3 face;
    size_t jp1 = (j + 1) % M;
    face[0] = static_cast<unsigned>(j);
    face[1] = static_cast<unsigned>(jp1);
    face[2] = static_cast<unsigned>(NORTH);
    faces.push_back(face);
    fnormals.push_back(FaceNormal(face, vertices));
    face[0] = static_cast<unsigned>(M * (N - 2) + j);
    face[1] = static_cast<unsigned>(SOUTH);
    face[2] = static_cast<unsigned>(M * (N - 2) + jp1);
    faces.push_back(face);
    fnormals.push_back(FaceNormal(face, vertices));
  }

  vertex_uv.reserve(vertices.size());
  for (const glm::vec3 pos : vertices)
  {
    vertex_uv.emplace_back(calculateUV(pos, uvMapping));
  }

  mesh.vertex_material.resize(vertices.size(), -1);

  return mesh;
}

Mesh aiSceneToMesh(const aiScene* scene)
{
  assert(scene->mNumMeshes == 1);
  aiMesh &inMesh = *scene->mMeshes[0];
  Mesh outMesh;

  outMesh.vertices.resize(inMesh.mNumVertices);
  std::transform(
    inMesh.mVertices, 
    inMesh.mVertices + inMesh.mNumVertices, 
    outMesh.vertices.begin(),
    [](const aiVector3D& v)-> glm::vec3
    {
      return { v.x, v.y, v.z };
    }
  );

  outMesh.vertex_normals.resize(inMesh.mNumVertices);
  std::transform(
    inMesh.mNormals,
    inMesh.mNormals + inMesh.mNumVertices,
    outMesh.vertex_normals.begin(),
    [](const aiVector3D& v)-> glm::vec3
    {
      return { v.x, v.y, v.z };
    }
  );
  
  // TODO: FIX UV
  outMesh.vertex_uv.resize(inMesh.mNumVertices);
  /*std::transform(
    inMesh.mTextureCoords,
    inMesh.mTextureCoords + inMesh.mNumVertices,
    outMesh.vertex_uv.begin(),
    [](const aiVector3D& v)-> glm::uvec2
    {
      return { v.x, v.y };
    }
  );*/

  outMesh.faces.resize(inMesh.mNumFaces);
  std::transform(
    inMesh.mFaces,
    inMesh.mFaces + inMesh.mNumFaces,
    outMesh.faces.begin(),
    [](const aiFace& f)-> glm::uvec3
    {
      assert(f.mNumIndices == 3);
      return { f.mIndices[0],f.mIndices[1],f.mIndices[2] };
    }
  );

  outMesh.face_normals.resize(inMesh.mNumFaces);
  // TODO: Generate Face Normals

  outMesh.vertex_material.resize(inMesh.mNumVertices, -1);

  //CenterMesh(outMesh.vertices);
  return outMesh;
}

Mesh aiImportMesh(const std::string& path)
{
  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile(path, 
    aiProcess_GenSmoothNormals | aiProcess_Triangulate
  );
  if(!scene)
    std::cout << importer.GetErrorString() << std::endl;
  return aiSceneToMesh(scene);
}


ntg::bounds3 meshBounds(const Mesh& mesh)
{
  return std::accumulate(
    mesh.vertices.begin(), 
    mesh.vertices.end(), 
    ntg::bounds3(),
    ntg::bounds3::grow_static
  );
}

ntg::bounds3 meshBounds(const std::vector<Mesh>& meshes)
{
  ntg::bounds3 b;
  for (const Mesh& mesh : meshes)
  {
    b = b.grow(meshBounds(mesh));
  }
  return b;
}

void meshBoundsExpand(const std::vector<Mesh>& meshes, glm::vec3 &min, glm::vec3 &max)
{
  for (const Mesh& mesh : meshes)
  {
    for (const glm::vec3& v : mesh.vertices)
    {
      for (int i = 0; i < 3; ++i)
      {
        if (v[i] < min[i]) min[i] = v[i];
        if (v[i] > max[i]) max[i] = v[i];
      }
    }
  }
}

glm::mat4 CenterMeshTransform(const std::vector<Mesh>& meshes)
{
  glm::vec3 minCorner = glm::vec3(std::numeric_limits<float>::infinity());
  glm::vec3 maxCorner = glm::vec3(-std::numeric_limits<float>::infinity());
  for (const Mesh &mesh : meshes)
  {
    for (const glm::vec3& v : mesh.vertices)
    {
      for (int i = 0; i < 3; ++i)
      {
        if (v[i] < minCorner[i]) minCorner[i] = v[i];
        if (v[i] > maxCorner[i]) maxCorner[i] = v[i];
      }
    }
  }
  
  glm::vec3 bb_scale = maxCorner - minCorner;
  float max_distance = *std::max_element(glm::data(bb_scale), glm::data(bb_scale) + 3);
  return glm::scale(1.f / max_distance) * translate(-(maxCorner + minCorner) / 2.f);
}

glm::mat4 CenterMeshTransform(const glm::vec3& minCorner, const glm::vec3& maxCorner)
{
  glm::vec3 bb_scale = maxCorner - minCorner;
  float max_distance = *std::max_element(glm::data(bb_scale), glm::data(bb_scale) + 3);
  return glm::scale(1.f / max_distance) * translate(-(maxCorner + minCorner) / 2.f);
}

glm::mat4 CenterMeshTransform(const ntg::bounds3& bounds)
{
  glm::vec3 size = bounds.size();

  float max_distance = 0;
  for (float v : size)
    max_distance = glm::max(max_distance, abs(v));
  return glm::scale(1.f / max_distance) * translate(-bounds.center());
}

void MeshTransform(std::vector<Mesh>& meshes, const glm::mat4& transform)
{
  for (Mesh& mesh : meshes)
    for (glm::vec3& v : mesh.vertices)
      v = transform * glm::vec4(v, 1);
}

void ImportMeshList(const std::string& path, const std::string& file, std::vector<Mesh>& meshes,
  std::vector<std::string>& names)
{
  std::cout << "importing children meshes from: " << file << std::endl;
  std::ifstream list(path + file);

  // load meshes
  while (!list.eof())
  {
    names.emplace_back();
    list >> names.back();
    if (names.back().empty())
    {
      names.pop_back();
      break;
    }
    std::cout << "importing: " << names.back() << std::endl;
    meshes.emplace_back(aiImportMesh(path + names.back()));
  }
}
