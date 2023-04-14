/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Plane.h
Purpose: Plane primitive class h file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 10 Feb 2021
End Header --------------------------------------------------------*/

#pragma once
#include <glm.hpp>
#include <vector>
#include <glew.h>
#include "Shader.h"

class AABB;
class BoundingSphere;
class Triangle;

class Plane
{
public:
	Plane() {}
	void Initialize(glm::vec3 normal, float dis);
	void Initialize(glm::vec3 a, glm::vec3 b, glm::vec3 c);

	void Draw(Shader* shader, glm::vec3 color);

	bool PlaneVsAABB(const AABB& aabb) const;
	bool PlaneVsBoundingSphere(const BoundingSphere& bc) const;
	bool PlaneVsTriangle(const Triangle& t) const;
	glm::vec3 GetClosestPoint(const glm::vec3 p) const;
	float PlaneVsPoint(const glm::vec3 p) const;

public:
	// (n.x, n.y, n.z, d)
	// Ax + By + Cz = d
	glm::vec3 normal = glm::vec3(0);
	float distance = 0;

//private:
//	void InitVAO();
//private:
//	std::vector<glm::vec3> points;
//	glm::mat4 modelmatrix = glm::mat4(1.f);
//	unsigned nrOfPoints = 0;
//
//	GLuint VAO = 0, VBO = 0;
};