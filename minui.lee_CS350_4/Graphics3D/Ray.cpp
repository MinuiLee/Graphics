/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Ray.h
Purpose: Ray primitive class cpp file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 10 Feb 2021
End Header --------------------------------------------------------*/

#include <algorithm>
#include "Ray.h"
#include "BoundingSphere.h"
#include "Plane.h"
#include "AABB.h"
#include "Triangle.h"

void Ray::Draw(Shader* shader, glm::vec3 color)
{
	shader->SetMat4fv(modelMatrix, "ModelMatrix");
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

bool Ray::RayVsPlane(const Plane& p) const
{
	float a = -glm::dot(p.normal, start - p.distance);
	float b = glm::dot(p.normal, direction);

	if (fabs(b) < FLT_EPSILON)
	{
		if (a != 0.f) return false;
		else return true;
	}

	float t = -a / b;
	if (t < 0.f)
		return false;


	return true;
}

bool Ray::RayVsAABB(const AABB& aabb) const
{
	float t1 = (aabb.min.x - start.x) / direction.x;
	float t2 = (aabb.max.x - start.x) / direction.x;
	float t3 = (aabb.min.y - start.y) / direction.y;
	float t4 = (aabb.max.y - start.y) / direction.y;
	float t5 = (aabb.min.z - start.z) / direction.z;
	float t6 = (aabb.max.z - start.z) / direction.z;

	float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
	float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

	// intersecting but AABB is behind  ||  not intersect
	if (tmax < 0 || tmin > tmax)
		return false;

	return true;
}

bool Ray::RayVsSphere(const BoundingSphere& bc) const
{
	glm::vec3 m = start - bc.center;
	float b = glm::dot(m, direction);
	float c = glm::dot(m, m) - (bc.radius * bc.radius);

	if (c > 0.f && b > 0.f) return false;

	float discriminant = b * b - c;

	if (discriminant < 0.f) return false;

	return true;
}

bool Ray::RayVsTriangle(const Triangle& triangle) const
{
	glm::vec3 u = triangle.v2 - triangle.v1;
	glm::vec3 v = triangle.v3 - triangle.v1;
	glm::vec3 n = glm::cross(u, v);

	if (n == glm::vec3(0))
		return false;

	glm::vec3 w0 = start - triangle.v1;
	float a = -glm::dot(n, w0);
	float b = glm::dot(n, direction);

	if (fabs(b) < FLT_EPSILON)
	{
		if (a != 0.f) return false;
		else return true;
	}

	float r = a / b;
	if (r < 0.f)
		return false;

	//----- plane ^^
	//----- in triangle VV

	glm::vec3 I = start + r * direction;

	float uu = glm::dot(u, u);
	float uv = glm::dot(u, v);
	float vv = glm::dot(v, v);
	glm::vec3 w = I - triangle.v1;
	float wu = glm::dot(w, u);
	float wv = glm::dot(w, v);
	float det = uv * uv - uu * vv;

	float s, t;
	s = (uv * wv - vv * wu) / det;
	if (s < 0.0 || s > 1.0)
		return false;

	t = (uv * wu - uu * wv) / det;
	if (t < 0.0 || (s + t > 1.f))
		return false;

	return true;
}

void Ray::InitVAO()
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

void Ray::Initialize(glm::vec3 start_, glm::vec3 dir)
{
	start = start_;
	direction = glm::normalize(dir);

	points.clear();

	points.push_back(start);
	points.push_back(start + dir * 500.f);

	nrOfPoints = static_cast<unsigned int>(points.size());
	InitVAO();
}
