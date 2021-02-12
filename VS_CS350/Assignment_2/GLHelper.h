#pragma once
#include <vector>
#include <GL/glew.h>

namespace GLHelper
{
	template<typename T>
	GLuint GenBuffer(const std::vector<T>& buffer, GLenum bufferType = GL_ARRAY_BUFFER)
	{
		GLuint buffer_id;
		glGenBuffers(1, &buffer_id);
		glBindBuffer(bufferType, buffer_id);

		glBufferData(
			bufferType,
			sizeof(T) * buffer.size(),
			buffer.data(),
			GL_STATIC_DRAW
		);
		return buffer_id;
	}

	template<glm::length_t L>
	void BindVertexAttribute(GLuint buffer_id, GLint attribute_id)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
		glVertexAttribPointer(attribute_id, L, GL_FLOAT, false, 0, nullptr);
		glEnableVertexAttribArray(attribute_id);
	}

	template<glm::length_t L, typename T>
	void BindVertexAttribute(
		GLuint buffer_id, 
		GLint attribute_id, 
		const std::vector<glm::vec<L, T>>&buffer)
	{
		BindVertexAttribute<L>(buffer_id, attribute_id);
	}
}
