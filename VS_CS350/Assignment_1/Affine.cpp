/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Affine.cpp
Purpose: basic math for affine transformations
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#include "Affine.h"


glm::vec4 point(float x, float y, float z)
{
	return { x,y,z,1.f };
}

glm::vec4 vector(float x, float y, float z)
{
	return { x,y,z,0.f };
}

bool isPoint(const glm::vec4& v)
{
	return near(v.w ,1.f);
}

bool isVector(const glm::vec4& v)
{
	return near(v.w ,0.f);
}

float angle(const glm::vec4& u, const glm::vec4& v)
{
	return glm::degrees(acos(dot(u, v) / (length(u) * length(v))));
}

glm::vec4 cross(const glm::vec4& u, const glm::vec4& v)
{
	return
	{
		u.y*v.z - u.z*v.y,
		u.z*v.x - u.x*v.z,
		u.x*v.y - u.y*v.x,
		0.f
	};
}

glm::mat4 affine(const glm::vec4& Lx, const glm::vec4& Ly, const glm::vec4& Lz, const glm::vec4& C)
{
	return { Lx,Ly,Lz,C };
}

bool isAffine(const glm::mat4& M)
{
	return	M[0].w == 0 && 
			M[1].w == 0 && 
			M[2].w == 0 && 
			M[3].w == 1;
}

glm::mat4 rotate(float deg, const glm::vec4& v)
{
	const float cosine = cos(glm::radians(deg));
	const glm::mat3 m1 = (1 - cosine) * outerProduct(glm::vec3(v), glm::vec3(v)) / dot(v, v);
	const glm::mat3 m2(cosine);

	const glm::mat3 cv =
	{
		0,    v.z,  -v.y,
		-v.z, 0,    v.x,
		v.y,  -v.x, 0,    
	};
	const glm::mat3 m3 = sin(glm::radians(deg)) / length(v) * cv;

	return { m1 + m2 + m3 };
}

glm::mat4 scale(float r)
{
	glm::mat4 output(r);
	output[3][3] = 1;
	return output;
}

glm::mat4 scale(float rx, float ry, float rz)
{
	return
	{
		rx, 0,  0,  0,
		0,  ry, 0,  0,
		0,  0,  rz, 0,
		0,  0,  0,  1
	};
}

glm::mat4 scale(const glm::vec4& v)
{
	return scale(v.x, v.y, v.z);
}

glm::mat4 translate(const glm::vec4& v)
{
	glm::mat4 output(1);
	output[3] = v;
	output[3][3] = 1;
	return output;
}

glm::mat4 affineInverse(const glm::mat4& A)
{
	return glm::mat4(inverse(glm::mat3(A))) * translate(-A[3]);
}
