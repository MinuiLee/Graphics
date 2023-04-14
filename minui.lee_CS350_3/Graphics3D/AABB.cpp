#include "AABB.h"
/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AABB.h
Purpose: AABB primitive class cpp file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 10 Feb 2021
End Header --------------------------------------------------------*/

#include "AABB.h"
#include "BoundingSphere.h"

void AABB::Draw(Shader* shader, glm::vec3 color)
{
	shader->SetMat4fv(modelmatrix, "ModelMatrix");
	shader->SetVec3f(color, "line_color");

	shader->Use();

	//Bind VAO
	glBindVertexArray(VAO);

	//Draw
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(points.size()));

	//CleanUp
	glBindVertexArray(0);
	glUseProgram(0);
}

bool AABB::TestAABBvsSphere(const BoundingSphere& bc) const
{
	glm::vec3 closestPnt = GetClosestPoint(bc.center);
	glm::vec3 diff = bc.center - closestPnt;

	float disSquare = glm::length(diff) * glm::length(diff);
	float radSquare = bc.radius * bc.radius;

	return disSquare < radSquare;
}

bool AABB::TestAABBvsAABB(const AABB& aabb) const
{
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (max[i] < aabb.min[i] || aabb.max[i] < min[i])
		{
			return false;
		}
	}

	return true;
}

glm::vec3 AABB::GetClosestPoint(const glm::vec3 point) const
{
	glm::vec3 closestPnt;

	// set x
	if (point.x > max.x)
		closestPnt.x = max.x;
	else if (point.x < min.x)
		closestPnt.x = min.x;
	else
		closestPnt.x = point.x;

	// set y
	if (point.y > max.y)
		closestPnt.y = max.y;
	else if (point.y < min.y)
		closestPnt.y = min.y;
	else
		closestPnt.y = point.y;

	// set z
	if (point.z > max.z)
		closestPnt.z = max.z;
	else if (point.z < min.z)
		closestPnt.z = min.z;
	else
		closestPnt.z = point.z;


	return closestPnt;
}

void AABB::InitVAO()
{
	//Create VAO
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Gen VBO and bind and send data
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nrOfPoints * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

	//Set vertex attribute pointers and enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	//Bind VAO 0
	glBindVertexArray(0);
}

AABB::AABB(glm::vec3 min_, glm::vec3 max_)
{
	Initialize(min_, max_);
}

void AABB::Initialize(glm::vec3 min_, glm::vec3 max_)
{
	max = max_;
	min = min_;

	if (min.x > max.x)
	{
		min.x = max_.x;
		max.x = min_.x;
	}

	if (min.y > max.y)
	{
		min.y = max_.y;
		max.y = min_.y;
	}

	if (min.z > max.z)
	{
		min.z = max_.z;
		max.z = min_.z;
	}

	modelmatrix = glm::mat4(1.f);

	points.clear();

	// top
	points.push_back(glm::vec3(min.x, max.y, min.z));
	points.push_back(glm::vec3(max.x, max.y, min.z));

	points.push_back(glm::vec3(max.x, max.y, min.z));
	points.push_back(glm::vec3(max.x, max.y, max.z));

	points.push_back(glm::vec3(max.x, max.y, max.z));
	points.push_back(glm::vec3(min.x, max.y, max.z));

	points.push_back(glm::vec3(min.x, max.y, max.z));
	points.push_back(glm::vec3(min.x, max.y, min.z));

	// bottom
	points.push_back(glm::vec3(min.x, min.y, min.z));
	points.push_back(glm::vec3(max.x, min.y, min.z));

	points.push_back(glm::vec3(max.x, min.y, min.z));
	points.push_back(glm::vec3(max.x, min.y, max.z));

	points.push_back(glm::vec3(max.x, min.y, max.z));
	points.push_back(glm::vec3(min.x, min.y, max.z));

	points.push_back(glm::vec3(min.x, min.y, max.z));
	points.push_back(glm::vec3(min.x, min.y, min.z));

	// left
	points.push_back(glm::vec3(min.x, max.y, max.z));
	points.push_back(glm::vec3(min.x, max.y, min.z));

	points.push_back(glm::vec3(min.x, max.y, min.z));
	points.push_back(glm::vec3(min.x, min.y, min.z));

	points.push_back(glm::vec3(min.x, min.y, min.z));
	points.push_back(glm::vec3(min.x, min.y, max.z));

	points.push_back(glm::vec3(min.x, min.y, max.z));
	points.push_back(glm::vec3(min.x, max.y, max.z));

	// right
	points.push_back(glm::vec3(max.x, max.y, max.z));
	points.push_back(glm::vec3(max.x, max.y, min.z));

	points.push_back(glm::vec3(max.x, max.y, min.z));
	points.push_back(glm::vec3(max.x, min.y, min.z));

	points.push_back(glm::vec3(max.x, min.y, min.z));
	points.push_back(glm::vec3(max.x, min.y, max.z));

	points.push_back(glm::vec3(max.x, min.y, max.z));
	points.push_back(glm::vec3(max.x, max.y, max.z));

	nrOfPoints = static_cast<unsigned int>(points.size());
	InitVAO();
}
