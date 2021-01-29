/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: a 3D camera class
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#include "Camera.h"
#include "Affine.h"
#include "VertexGlobalSystem.h"


namespace
{
	glm::mat4 dirTransform(glm::mat4 L)
	{
		return transpose(inverse(glm::mat3(L)));
	}
}


Camera::Camera() :
	eye_point(point(0, 0, 0)),
	right_vector(vector(1, 0, 0)),
	up_vector(vector(0, 1, 0)),
	back_vector(vector(0, 0, 1)),
	vp_width(0.2f),
	vp_height(0.2f),
	vp_distance(0.1f),
	near_distance(0.1f),
	far_distance(10.f)
{}

Camera::Camera(const glm::vec4& E, const glm::vec4& look, const glm::vec4& rel, float fov, float aspect, float N,
	float F) :
	eye_point(E),
	right_vector(normalize(cross(look, rel))),
	up_vector(cross(normalize(-look), right_vector)),
	back_vector(normalize(-look)),
	vp_width(2 * N*glm::tan(glm::radians(fov / 2))),
	vp_height(vp_width / aspect),
	vp_distance(N),
	near_distance(N),
	far_distance(F)
{}

glm::vec4 Camera::viewport() const
{
	return vector(vp_width, vp_height, vp_distance);
}

Camera& Camera::zoom(float factor)
{
	vp_width *= factor;
	vp_height *= factor;
	return *this;
}

Camera& Camera::forward(float distance)
{
	eye_point += -distance * back_vector;
	return *this;
}

Camera& Camera::yaw(float angle)
{
	glm::mat4 M = dirTransform(rotate(angle, up_vector));
	right_vector = M * right_vector;
	back_vector = M * back_vector;
	return *this;
}

Camera& Camera::pitch(float angle)
{
	glm::mat4 M = dirTransform(rotate(angle, right_vector));
	up_vector = M * up_vector;
	back_vector = M * back_vector;
	return *this;
}

Camera& Camera::roll(float angle)
{
	glm::mat4 M = dirTransform(rotate(angle, back_vector));
	right_vector = M * right_vector;
	up_vector = M * up_vector;
	return *this;
}


void Camera::UpdateGPUCamera() const
{
	VertexGlobalSystem::SetWorldToCam(view(*this));
	VertexGlobalSystem::SetCamToNDC(perspective(*this));
}
