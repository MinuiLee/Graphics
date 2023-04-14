/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Ray.h
Purpose: Ray primitive class h file
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
class Triangle;
class Plane;
class BoundingSphere;

class Ray
{
public:
	Ray() {}
	void Initialize(glm::vec3 start, glm::vec3 dir);

	void Draw(Shader* shader, glm::vec3 color);

	bool RayVsPlane(const Plane& plane) const;
	bool RayVsAABB(const AABB& aabb) const;
	bool RayVsSphere(const BoundingSphere& bc ) const;
	bool RayVsTriangle(const Triangle& triangle) const;

public:
	glm::vec3 start = glm::vec3(0.f);
	glm::vec3 direction = glm::vec3(0.f);

private:
	void InitVAO();

private:
	std::vector<glm::vec3> points;
	glm::mat4 modelMatrix = glm::mat4(1.f);
	unsigned nrOfPoints = 0;

	GLuint VAO = 0, VBO = 0;
};