/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: FaceNormalRender.cpp
Purpose: renderer for displaying the normals of a meshes faces
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#include "FaceNormalRender.h"
#include <iostream>
#include <string>
#include <fstream>
#include <numeric>
#include <streambuf>
#include <stdexcept>

#include "Camera.h"
#include "GLHelper.h"
#include "shader.hpp"

FaceNormalRender::FaceNormalRender()
{

	// Create and compile our GLSL program from the shaders
	program = LoadShaders("../../Common/shaders/Forward/LineRender.vert",
		"../../Common/shaders/Forward/LineRender.frag");

	// turn on depth buffer
	glEnable(GL_DEPTH_TEST);

	// shader parameter locations
	upersp_matrix = glGetUniformLocation(program, "persp_matrix");
	uview_matrix = glGetUniformLocation(program, "view_matrix");
	umodel_matrix = glGetUniformLocation(program, "model_matrix");
	ucolor = glGetUniformLocation(program, "color");

	glUseProgram(program);
}

FaceNormalRender::~FaceNormalRender()
{
	glUseProgram(0);
	glDeleteProgram(program);
}

void FaceNormalRender::clear(const glm::vec4& color)
{
	glClearColor(color.x, color.y, color.z, color.w);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

FaceNormalRender& FaceNormalRender::Instance()
{
	static FaceNormalRender renderer;
	return renderer;
}

int FaceNormalRender::loadMesh(const Mesh& m, float line_length)
{
	MeshData data;

	// references to inside of data to avoid rewriting code
	GLuint& vertex_buffer = data.buffer_objects[MeshData::VERT];
	GLuint& line_buffer = data.buffer_objects[MeshData::LINE];
	GLuint& vao = data.vertex_array_buffer;
	int& line_count = data.line_count;

	std::vector<glm::vec3> vertices(m.faces.size() * 2);
	for (size_t i = 0; i < m.faces.size() * 2; i += 2)
	{
		const auto& face = m.faces[i/2];
		vertices[i] = (m.vertices[face[0]] + m.vertices[face[1]] + m.vertices[face[2]]) / 3.f;
		vertices[i + 1] = vertices[i] + (normalize(m.face_normals[i / 2]) * line_length);
	}

	std::vector<unsigned> lines(m.faces.size() * 2);
	std::iota(lines.begin(), lines.end(), 0);

	vertex_buffer = GLHelper::GenBuffer(vertices);
	line_buffer = GLHelper::GenBuffer(lines, GL_ELEMENT_ARRAY_BUFFER);

	// create vertex array object
	glGenVertexArrays(1, &vao);
	//   start state recording
	glBindVertexArray(vao);

	// assign postion attribute to vertex_buffer
	GLHelper::BindVertexAttribute(vertex_buffer, 0, vertices);

	// ready the line buffer for drawing
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_buffer);
	//   stop state recording
	glBindVertexArray(0);

	line_count = static_cast<int>(lines.size());

	Instance().mesh_data.push_back(data);
	return static_cast<int>(Instance().mesh_data.size() - 1);
}

void FaceNormalRender::unloadMesh(int mi)
{

	// references to inside of data to avoid rewriting code
	MeshData& data = Instance().mesh_data[mi];
	GLuint& vertex_buffer = data.buffer_objects[MeshData::VERT];
	GLuint& line_buffer = data.buffer_objects[MeshData::LINE];
	GLuint& vao = data.vertex_array_buffer;

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &line_buffer);
	glDeleteBuffers(1, &vertex_buffer);
}

void FaceNormalRender::setModel(const glm::mat4& M)
{
	glUseProgram(Instance().program);
	glUniformMatrix4fv(Instance().umodel_matrix, 1, false, glm::data(M));
}

void FaceNormalRender::setCamera(const Camera& cam)
{
	glUseProgram(Instance().program);
	glUniformMatrix4fv(Instance().uview_matrix, 1, false, glm::data(view(cam)));
	glUniformMatrix4fv(Instance().upersp_matrix, 1, false, glm::data(perspective(cam)));
}

void FaceNormalRender::setRGB(const glm::vec4& color)
{
	glUseProgram(Instance().program);
	glUniform4fv(Instance().ucolor, 1, glm::data(color));
}

void FaceNormalRender::draw(int mi)
{
	glUseProgram(Instance().program);
	// references to inside of data to avoid rewriting code
	MeshData& data = Instance().mesh_data[mi];
	GLuint& vao = data.vertex_array_buffer;
	int& line_count = data.line_count;

	// recall state
	glBindVertexArray(vao);
	glLineWidth(3.0f);
	glDrawElements(GL_LINES, 2 * line_count, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
