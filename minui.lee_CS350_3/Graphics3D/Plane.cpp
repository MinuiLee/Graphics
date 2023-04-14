/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Plane.h
Purpose: Plane primitive class cpp file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 10 Feb 2021
End Header --------------------------------------------------------*/

#include "Plane.h"
#include "BoundingSphere.h"
#include "AABB.h"
#include "Triangle.h"

void Plane::Draw(Shader* shader, glm::vec3 color)
{
	//shader->SetMat4fv(modelmatrix, "ModelMatrix");
	//shader->SetVec3f(color, "line_color");

	//shader->Use();

	////Bind VAO
	//glBindVertexArray(VAO);

	////Draw
	//glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(points.size()));

	////CleanUp
	//glBindVertexArray(0);
	//glUseProgram(0);
}

bool Plane::PlaneVsAABB(const AABB& aabb) const
{
	//center-extents
	glm::vec3 center = (aabb.max + aabb.min) / 2.f;
	glm::vec3 extent = aabb.max - center;

	float r = extent.x * fabs(normal.x) + extent.y * fabs(normal.y) + extent.z * fabs(normal.z);
	
	//distance of aabb center from plane
	float d = glm::dot(normal, center) - distance;

	return fabs(d) <= r;
}

bool Plane::PlaneVsBoundingSphere(const BoundingSphere& bc) const
{
	glm::vec3 closestPnt = GetClosestPoint(bc.center);
	float disFromSphere = glm::distance(bc.center, closestPnt);

	return disFromSphere <= bc.radius;
}

bool Plane::PlaneVsTriangle(const Triangle& t) const
{
	float a = glm::dot(normal, t.v1);
	float b = glm::dot(normal, t.v2);
	float c = glm::dot(normal, t.v3);

	if (a == 0 && b == 0 && c == 0) return true; // on plane
	
	if (a > 0.f && b > 0.f && c > 0.f) return false;
	if (a < 0.f && b < 0.f && c < 0.f) return false;

	return true;
}

glm::vec3 Plane::GetClosestPoint(const glm::vec3 point) const
{
	glm::vec3 normal(normal);

	float dis = glm::dot(normal, point);

	return point - dis * normal;
}

float Plane::PlaneVsPoint(const glm::vec3 p) const
{
	return  glm::dot(normal, p) - distance;
}

//void Plane::InitVAO()
//{
//	//Create VAO
//	glCreateVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//	//Gen VBO and bind and send data
//	glGenBuffers(1, &VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, nrOfPoints * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);
//
//	//Set vertex attribute pointers and enable
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	//Bind VAO 0
//	glBindVertexArray(0);
//}

void Plane::Initialize(glm::vec3 normal_, float dis_)
{
	normal = glm::normalize(normal_);
	distance = dis_;
	
	//points.clear();

	//glm::vec3 forward = glm::vec3(normal.x, normal.y, normal.z);
	//glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
	//glm::vec3 right = glm::cross(forward, up);
	//up = glm::cross(right, forward);

	//glm::normalize(forward);
	//glm::normalize(right);
	//glm::normalize(up);

	//glm::mat4 rot = glm::mat4(
	//	right.x, up.x, -forward.x, 0.f,
	//	right.y, up.y, -forward.y, 0.0f,
	//	right.z, up.z, -forward.z, 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f);

	//modelmatrix = glm::mat4(1.f);
	//modelmatrix = glm::translate(modelmatrix, normal * distance);
	//modelmatrix *= rot;
	//modelmatrix = glm::scale(modelmatrix, glm::vec3(3.f)); //1.f -> scale
	//
	//points.push_back(glm::vec3(-1, -1, 0));
	//points.push_back(glm::vec3(-1, 1, 0));
	//points.push_back(glm::vec3(1, 1, 0));
	//points.push_back(glm::vec3(1, -1, 0));

	//nrOfPoints = static_cast<unsigned int>(points.size());
	//InitVAO();
}

void Plane::Initialize(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 nor = glm::cross(b - a, c - a);
	Initialize(nor, glm::dot(nor, a));
}
