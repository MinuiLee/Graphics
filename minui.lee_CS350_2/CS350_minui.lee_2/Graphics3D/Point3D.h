/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Point3D.h
Purpose: Point3D primitive class h file
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

class Point3D
{
public:
	Point3D() {}
	void Initialize(glm::vec3 coord);

	void Draw(Shader* shader, glm::vec3 color);

	bool PointVsSphere(const BoundingSphere& bc) const;
	bool PointVsAABB(const AABB& aabb) const;
	bool PointVsTriangle(const Triangle& triangle) const;
	bool PointVsPlane(const Plane& plane) const;

public:
	glm::vec3 coord = glm::vec3(0.f);

private:
	void InitVAO();
private:
	std::vector<glm::vec3> points;
	glm::mat4 modelmatrix = glm::mat4(1.f);
	unsigned nrOfPoints = 0;

	GLuint VAO = 0, VBO = 0;
};