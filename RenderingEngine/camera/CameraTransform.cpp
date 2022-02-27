/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: CameraTransform.cpp
Purpose: camera functionality used by renderers
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#include "Camera.h"

glm::mat4 model(const Camera& cam)
{
	return glm::mat4{
	  glm::vec4(cam.right(),0),
		glm::vec4(cam.up(),0),
		glm::vec4(cam.back(),0),
		glm::vec4(cam.eye(),1),
	};
}

glm::mat4 view(const Camera& cam)
{
	return glm::inverse(model(cam));
}

glm::mat4 perspective(const Camera& cam)
{
	const float W = cam.viewport().x;
	const float H = cam.viewport().y;
	const float D = cam.viewport().z;
	const float A = (    cam.near() + cam.far()) / (cam.near() - cam.far());
	const float B = (2 * cam.near() * cam.far()) / (cam.near() - cam.far());

	return
	transpose(glm::mat4{
		2*D/W, 0,     0,     0,
		0,     2*D/H, 0,     0,
		0,     0,     A,     B,
		0,     0,     -1,    0
	});
}