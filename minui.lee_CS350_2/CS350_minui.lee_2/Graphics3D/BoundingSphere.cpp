/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BoundingSphere.h
Purpose: BoundingSphere primitive class cpp file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 10 Feb 2021
End Header --------------------------------------------------------*/

#include "BoundingSphere.h"
#include "AABB.h"

constexpr float PI = glm::pi<float>();

void BoundingSphere::Draw(Shader* shader, glm::vec3 color)
{
	shader->SetMat4fv(modelmatrix, "ModelMatrix");
	shader->SetVec3f(color, "line_color");

	shader->Use();

	//Bind VAO
	glBindVertexArray(VAO);

	//Draw
	glDrawElements(GL_LINES, nrOfIndices, GL_UNSIGNED_INT, nullptr); 

	//CleanUp
	glBindVertexArray(0);
	glUseProgram(0);
}

bool BoundingSphere::TestSphereVsSphere(const BoundingSphere& bc ) const
{
	return glm::distance(center, bc.center) <= (radius + bc.radius);
}

bool BoundingSphere::TestSphereVsAABB(const AABB& aabb) const
{
	glm::vec3 closestPnt = aabb.GetClosestPoint(center);
	glm::vec3 diff = center - closestPnt;

	float disSquare = glm::length(diff) * glm::length(diff);
	float radSquare = radius * radius;

	return disSquare < radSquare;
}

void BoundingSphere::InitVAO()
{
	//Create VAO
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Gen VBO and bind and send data
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nrOfPoints * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

	//Gen EBO and bind and send data
	if (nrOfIndices > 0)
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nrOfIndices * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	}

	//Set vertex attribute pointers and enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	//Bind VAO 0
	glBindVertexArray(0);
}

void BoundingSphere::Initialize(glm::vec3 pos, float rad)
{
	center = pos;
	radius = rad;

	points.clear();

	int divTheta = 10;
	int divPhi = 10;

	float thetaStep = 2.f * PI / static_cast<float>(divTheta);
	float phiStep = PI / static_cast<float>(divPhi);
	float thetaAngle, phiAngle;

	for (int i = 0; i <= divPhi; ++i)
	{
		phiAngle = PI / 2 - i * phiStep;

		for (int j = 0; j <= divTheta; ++j)
		{
			thetaAngle = j * thetaStep;

			glm::vec3 v;
			v.x = radius * cosf(phiAngle) * cosf(thetaAngle);
			v.y = radius * cosf(phiAngle) * sinf(thetaAngle);
			v.z = radius * sinf(phiAngle);
			v += pos;

			points.push_back(v);
		}
	}

	int one, two;
	for (int i = 0; i < divPhi; ++i)
	{
		one = i * (divTheta + 1);
		two = one + divTheta + 1;
		for (int j = 0; j < divTheta; ++j, ++one, ++two)
		{
			if (i != 0)
			{
				indices.push_back(one);
				indices.push_back(two);
				indices.push_back(one + 1);
			}

			if (i != (divPhi - 1))
			{
				indices.push_back(one + 1);
				indices.push_back(two);
				indices.push_back(two + 1);
			}
		}
	}

	nrOfPoints = static_cast<unsigned int>(points.size());
	nrOfIndices = static_cast<unsigned int>(indices.size());
	InitVAO();
}