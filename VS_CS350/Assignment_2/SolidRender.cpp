/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongRender.cpp
Purpose: 3D lighting renderer
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#include "SolidRender.h"
#include <streambuf>


#include "GLHelper.h"
#include "shader.hpp"
#include "LightSystem.h"


SolidRender::SolidRender()
{
	// Create and compile our GLSL programs from the shaders
	LoadShaders();

	program_ = PHONG_SHADING;

	udiffuseTex = glGetUniformLocation(programs_[program_], "diffuseTexture");
	uSpecularTex = glGetUniformLocation(programs_[program_], "specularTexture");

	// turn on depth buffer
	glEnable(GL_DEPTH_TEST);
}

SolidRender::~SolidRender()
{
	glUseProgram(0);
	for(int i = 0; i < 4; ++i)
	  glDeleteProgram(programs_[i]);
}

void SolidRender::clear(const glm::vec4& color)
{
	glClearColor(color.x, color.y, color.z, color.w);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

SolidRender& SolidRender::Instance()
{
	static SolidRender renderer;
	return renderer;
}

int SolidRender::loadMesh(const Mesh& m, SHADER program)
{
	MeshData data;

	// references to inside of data to avoid rewriting code
	GLuint& vertex_buffer = data.bufferObjects_[MeshData::VERT];
	GLuint& normal_buffer = data.bufferObjects_[MeshData::NORM];
	GLuint& face_buffer = data.bufferObjects_[MeshData::FACE];
	GLuint& uv_buffer = data.bufferObjects_[MeshData::UV];
	GLuint& vao = data.vertexArrayBuffer_;
	int& face_count = data.faceCount_;

	vertex_buffer = GLHelper::GenBuffer(m.vertices);
	normal_buffer = GLHelper::GenBuffer(m.vertex_normals);
	uv_buffer = GLHelper::GenBuffer(m.vertex_uv);
	face_buffer = GLHelper::GenBuffer(m.faces, GL_ELEMENT_ARRAY_BUFFER);

	// create vertex array object
	glGenVertexArrays(1, &vao);
	//   start state recording
	glBindVertexArray(vao);

	GLHelper::BindVertexAttribute(vertex_buffer, 0, m.vertices);
	GLHelper::BindVertexAttribute(normal_buffer, 1, m.vertex_normals);
	GLHelper::BindVertexAttribute(uv_buffer, 2, m.vertex_uv);

	// ready the face buffer for drawing
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face_buffer);
	//   stop state recording
	glBindVertexArray(0);

	face_count = m.faces.size();

	Instance().meshData_.push_back(data);
	return static_cast<int>(Instance().meshData_.size() - 1);
}

void SolidRender::unloadMesh(int mi)
{

	// references to inside of data to avoid rewriting code
	MeshData &data = Instance().meshData_[mi];
	GLuint &vertex_buffer = data.bufferObjects_[MeshData::VERT];
	GLuint &normal_buffer = data.bufferObjects_[MeshData::NORM];
	GLuint &face_buffer = data.bufferObjects_[MeshData::FACE];
	GLuint &vao = data.vertexArrayBuffer_;

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &normal_buffer);
	glDeleteBuffers(1, &face_buffer);
	glDeleteBuffers(1, &vertex_buffer);
}

void SolidRender::draw(int mi, Texture& diffuseTex, Texture& specularTex)
{
	glUseProgram(GetProgram());

	// load diffuse texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTex.TextureID());
	glUniform1i(Instance().udiffuseTex, 0);

	// load specular texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTex.TextureID());
	glUniform1i(Instance().uSpecularTex, 1);

	// references to inside of data to avoid rewriting code
	MeshData& data = Instance().meshData_[mi];
	GLuint& vao = data.vertexArrayBuffer_;
	int& face_count = data.faceCount_;

	// recall state
	glBindVertexArray(vao);
	glLineWidth(3.0f);
	glDrawElements(GL_TRIANGLES, 3 * face_count, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

SolidRender::SHADER SolidRender::GetShader()
{
	return Instance().program_;
}

void SolidRender::SetShader(SHADER shadertype)
{
	Instance().program_ = shadertype;
}

void SolidRender::ReloadShaders()
{
	Instance().LoadShaders();
}

void SolidRender::LoadShaders()
{
	programs_[PHONG_LIGHTING] = ::LoadShaders("../../Common/shaders/Forward/PhongLighting.vert",
		"../../Common/shaders/Forward/SolidLighting.frag");
	programs_[PHONG_SHADING] = ::LoadShaders("../../Common/shaders/Forward/SolidShading.vert",
		"../../Common/shaders/Forward/PhongShading.frag");
	programs_[BLINN_LIGHTING] = ::LoadShaders("../../Common/shaders/Forward/BlinnLighting.vert",
		"../../Common/shaders/Forward/SolidLighting.frag");
	programs_[BLINN_SHADING] = ::LoadShaders("../../Common/shaders/Forward/SolidShading.vert",
		"../../Common/shaders/Forward/BlinnShading.frag");
	programs_[FLAT_EMISSION] = ::LoadShaders("../../Common/shaders/Forward/FlatEmission.vert",
		"../../Common/shaders/Forward/FlatEmission.frag");
	programs_[DEFFERED] = ::LoadShaders("../../Common/shaders/Forward/GBuffer.vert",
		"../../Common/shaders/Forward/GBuffer.frag");
}

GLuint SolidRender::GetProgram()
{
	SolidRender &instance = Instance();
	return instance.programs_[instance.program_];
}
