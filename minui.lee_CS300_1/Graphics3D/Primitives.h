/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Primitives.h
Purpose: Pre-defined primivies with vertices and indicies for meshes
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#pragma once

#include <vector>
#include <glew.h>
#include <glfw3.h>

constexpr float PI = glm::pi<float>();

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texcoord;
	glm::vec3 normal;
};

struct Point
{
	glm::vec3 position;
	glm::vec3 color;
};

class Primitive
{
public:
	Primitive() {}
	virtual ~Primitive() {}

	void set(const Vertex* vertices_, const unsigned nrOfVertices, const GLuint* indices_, const unsigned nrOfIndices)
	{
		for (size_t i = 0; i < nrOfVertices; i++)
		{
			vertices.push_back(vertices_[i]);
		}

		for (size_t i = 0; i < nrOfIndices; i++)
		{
			indices.push_back(indices_[i]);
		}
	}

	std::vector<Vertex> getVertices() { return vertices; }
	std::vector<GLuint> getIndices() { return indices; }
	const unsigned getNrOfVertices() { return vertices.size(); }
	const unsigned getNrOfIndices() { return indices.size(); }

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};

class Triangle : public Primitive
{
public:
	Triangle()
		: Primitive()
	{
		Vertex vertices_[] =
		{
			//Position								//Color							//Texcoords					//Normals
			glm::vec3(0.f, 0.5f, 0.f),				glm::vec3(1.f, 0.f, 0.f),		glm::vec2(0.5f, 1.f),		glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(-0.5f, -0.5f, 0.f),			glm::vec3(0.f, 1.f, 0.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.5f, -0.5f, 0.f),			glm::vec3(0.f, 0.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f),
		};
		unsigned nrOfVertices = sizeof(vertices_) / sizeof(Vertex);

		GLuint indices_[] =
		{
			0, 1, 2	//Triangle 1
		};
		unsigned nrOfIndices = sizeof(indices_) / sizeof(GLuint);

		set(vertices_, nrOfVertices, indices_, nrOfIndices);
	}
};

class Quad : public Primitive
{
public:
	Quad()
		: Primitive()
	{
		Vertex vertices_[] =
		{
			//Position								//Color							//Texcoords					//Normals
			glm::vec3(-0.5f, 0.5f, 0.f),			glm::vec3(1.f, 0.f, 0.f),		glm::vec2(0.f, 1.f),		glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(-0.5f, -0.5f, 0.f),			glm::vec3(0.f, 1.f, 0.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.5f, -0.5f, 0.f),			glm::vec3(0.f, 0.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.5f, 0.5f, 0.f),				glm::vec3(1.f, 1.f, 0.f),		glm::vec2(1.f, 1.f),		glm::vec3(0.f, 0.f, 1.f)
		};
		unsigned nrOfVertices = sizeof(vertices_) / sizeof(Vertex);

		GLuint indices_[] =
		{
			0, 1, 2,	//Triangle 1
			0, 2, 3		//Triangle 2
		};
		unsigned nrOfIndices = sizeof(indices_) / sizeof(GLuint);

		set(vertices_, nrOfVertices, indices_, nrOfIndices);
	}
};

class Sphere : public Primitive
{
public:
	Sphere(int divTheta, int divPhi, float radius) : Primitive()
	{
		float x, y, z;
		float thetaStep = 2.f * PI / static_cast<float>(divTheta);
		float phiStep = PI / static_cast<float>(divPhi);
		float thetaAngle, phiAngle;

		for (int i = 0; i <= divPhi; ++i)
		{
			phiAngle = PI / 2 - i * phiStep;

			for (int j = 0; j <= divTheta; ++j)
			{
				thetaAngle = j * thetaStep;
				Vertex v;
				x = radius * cosf(phiAngle) * cosf(thetaAngle);
				y = radius * cosf(phiAngle) * sinf(thetaAngle);
				z = radius * sinf(phiAngle);
				v.position = { x, y, z };
				v.color = glm::vec3(1.f);
				vertices.push_back(v);
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
	}
};
