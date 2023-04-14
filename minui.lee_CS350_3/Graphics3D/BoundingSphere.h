/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BoundingSphere.h
Purpose: BoundingSphere primitive class h file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 10 Feb 2021
End Header --------------------------------------------------------*/

#pragma once

#include <vector>
#include <glm.hpp>
#include <glew.h>
#include "Shader.h"

class AABB;

class BoundingSphere
{
public:
	BoundingSphere() {}
	void Initialize(glm::vec3 pos, float radius);

	void Draw(Shader* shader, glm::vec3 color);

	bool TestSphereVsSphere(const BoundingSphere& bc) const;
	bool TestSphereVsAABB(const AABB& aabb) const;

public:
	glm::vec3 center = glm::vec3(0);
	float radius = 0;

private:
	void InitVAO();
private:
	std::vector<glm::vec3> points;
	std::vector<GLuint> indices;
	unsigned nrOfPoints = 0;
	unsigned nrOfIndices = 0;

	glm::mat4 modelmatrix = glm::mat4(1.f);

	GLuint VAO = 0, VBO = 0, EBO = 0;
};