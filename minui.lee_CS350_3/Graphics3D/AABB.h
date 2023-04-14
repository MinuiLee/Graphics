/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AABB.h
Purpose: AABB primitive class h file
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

class BoundingSphere;

class AABB
{
public:
	AABB() {}
	AABB(glm::vec3 min_, glm::vec3 max_);

	void Initialize(glm::vec3 min, glm::vec3 max);

	void Draw(Shader* shader, glm::vec3 color);
	
	bool TestAABBvsSphere(const BoundingSphere& bc) const;
	bool TestAABBvsAABB(const AABB& aabb) const;
	glm::vec3 GetClosestPoint(const glm::vec3 point) const;

public:
	glm::vec3 min = glm::vec3(0.f);
	glm::vec3 max = glm::vec3(0.f);

private:
	void InitVAO();

private:
	std::vector<glm::vec3> points;
	glm::mat4 modelmatrix = glm::mat4(1.f);
	unsigned nrOfPoints = 0;

	GLuint VAO = 0;
	GLuint VBO = 0;
};