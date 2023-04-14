/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.cpp
Purpose: Mesh class cpp file which contains mesh properties such as vertices
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#include "Mesh.h"
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

Mesh::Mesh(std::vector<Vertex> vertexArray_, std::vector<GLuint> indexArray_)
{
	nrOfVertices = vertexArray_.size();
	nrOfIndices = indexArray_.size();

	for (size_t i = 0; i < nrOfVertices; i++)
	{
		vertexArray.push_back(vertexArray_[i]);
	}
	for (size_t i = 0; i < nrOfIndices; i++)
	{
		indexArray.push_back(indexArray_[i]);
	}

	SetNormals();
	InitVAO();
}

Mesh::Mesh(Primitive* primitive)
{
	nrOfVertices = primitive->getNrOfVertices();
	nrOfIndices = primitive->getNrOfIndices();

	for (size_t i = 0; i < nrOfVertices; i++)
	{
		Vertex v;
		v.position = primitive->getVertices()[i].position;
		v.color = primitive->getVertices()[i].color;
		vertexArray.push_back(v);
	}

	for (size_t i = 0; i < nrOfIndices; i++)
	{
		indexArray.push_back(primitive->getIndices()[i]);
	}

	SetNormals();
	InitVAO();
}

Mesh::Mesh(std::string objFile)
{
	std::stringstream ss;
	std::ifstream in_file(objFile);
	std::string line = "", prefix = "";
	GLint idx = 0;

	if (!in_file.is_open())
	{
		throw "RORROOROREOOEOEO";
	}

	while (std::getline(in_file, line))
	{
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "v")
		{
			Vertex v;
			ss >> v.position.x >> v.position.y >> v.position.z;
			v.color = glm::vec3(1.f);
			vertexArray.push_back(v);
		}
		else if (prefix == "f")
		{
			int counter = 0;
			std::vector<int> vi;

			while (ss >> idx)
			{
				if (counter == 0)
					vi.push_back(idx);

				if (ss.peek() == '/')
				{
					++counter;
					ss.ignore(1, '/');
				}
				else if (ss.peek() == ' ' && counter != 0)
				{
					++counter;
					ss.ignore(1, ' ');
				}
				if (ss.peek() == '/')
				{
					++counter;
					ss.ignore(1, '/');
				}

				if (counter > 2)
					counter = 0;
			}

			for (size_t i = 1; i < vi.size() - 1; ++i)
			{
				indexArray.push_back(vi[0] - 1);
				indexArray.push_back(vi[i] - 1);
				indexArray.push_back(vi[i + 1] - 1);
			}
		}
		prefix.clear();
	}

	nrOfVertices = vertexArray.size();
	nrOfIndices = indexArray.size();

	ResetVertices();
	SetNormals();
	InitVAO();
}

Mesh::Mesh(const Mesh& obj)
{
	nrOfVertices = obj.nrOfVertices;
	nrOfIndices = obj.nrOfIndices;

	for (size_t i = 0; i < nrOfVertices; i++)
		vertexArray.push_back(vertexArray[i]);

	for (size_t i = 0; i < nrOfIndices; i++)
		indexArray.push_back(obj.indexArray[i]);

	size_t size = obj.normal_lines.size();
	for (size_t i = 0; i < size; i++)
		normal_lines.push_back(obj.normal_lines[i]);

	size = obj.face_normals.size();
	for (size_t i = 0; i < size; i++)
		face_normals.push_back(obj.face_normals[i]);

	SetNormals();
	InitVAO();
}

Mesh::Mesh()
{
	SetOrbitPoints();
	InitVAO();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &VAO_nr);
	glDeleteBuffers(1, &VBO_nr);

	glDeleteVertexArrays(1, &VAO_fnr);
	glDeleteBuffers(1, &VBO_fnr);

	glDeleteVertexArrays(1, &VAO_or);
	glDeleteBuffers(1, &VBO_or);

	if (nrOfIndices > 0)
		glDeleteBuffers(1, &EBO);
}

void Mesh::Render(Shader* shader, GLenum mode)
{
	shader->Use();

	//Bind VAO
	glBindVertexArray(VAO);

	//Draw
	if (nrOfIndices == 0)
	{
		glDrawArrays(mode, 0, nrOfVertices);
	}
	else
	{
		glDrawElements(mode, nrOfIndices, GL_UNSIGNED_INT, nullptr); //0 == start from beginning
	}

	//CleanUp
	glBindVertexArray(0);
	glUseProgram(0);
	//glActiveTexture(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::RenderNormal(Shader* shader)
{
	shader->Use();

	//Bind VAO
	glBindVertexArray(VAO_nr);

	//Draw
	glDrawArrays(GL_LINES, 0, normal_lines.size());

	//CleanUp
	glBindVertexArray(0);
	glUseProgram(0);
	//glActiveTexture(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::RenderFaceNormal(Shader* shader)
{
	shader->Use();

	//Bind VAO
	glBindVertexArray(VAO_fnr);

	//Draw
	glDrawArrays(GL_LINES, 0, fnormal_lines.size());

	//CleanUp
	glBindVertexArray(0);
	glUseProgram(0);
	//glActiveTexture(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::RenderOrbit(Shader* shader)
{
	shader->Use();

	//Bind VAO
	glBindVertexArray(VAO_or);

	//Draw
	glDrawArrays(GL_LINES, 0, orbit_lines.size());

	//CleanUp
	glBindVertexArray(0);
	glUseProgram(0);
	//glActiveTexture(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::SetNormals()
{
	for (size_t i = 0; i < nrOfVertices; ++i)
	{
		vertexArray[i].normal = glm::vec3{ 0.f };
	}

	face_normals.resize(nrOfIndices, glm::fvec3());
	for (size_t i = 0; i < nrOfIndices; i += 3)
	{
		glm::fvec3 e1 = vertexArray[indexArray[i + 1]].position - vertexArray[indexArray[i]].position;
		glm::fvec3 e2 = vertexArray[indexArray[i + 2]].position - vertexArray[indexArray[i]].position;
		glm::fvec3 normal = glm::cross(e1, e2);

		face_normals[i] += normal;
		face_normals[i + 1] += normal;
		face_normals[i + 2] += normal;

		vertexArray[indexArray[i]].normal += normal;
		vertexArray[indexArray[i + 1]].normal += normal;
		vertexArray[indexArray[i + 2]].normal += normal;
	}

	for (size_t i = 0; i < nrOfIndices; ++i)
	{
		face_normals[i] = glm::normalize(face_normals[i]);
	}

	for (size_t i = 0; i < nrOfVertices; ++i)
	{
		vertexArray[i].normal = glm::normalize(vertexArray[i].normal);
	}

	for (size_t i = 0; i < nrOfVertices; ++i)
	{
		normal_lines.push_back({ vertexArray[i].position, glm::vec3{0.f,0.f,1.f} });
		normal_lines.push_back({ vertexArray[i].position + vertexArray[i].normal * 0.02f, glm::vec3{ 0.f,0.f,1.f } });
	}

	for (size_t i = 0; i < nrOfIndices; i += 3)
	{
		glm::vec3 fCenter = glm::vec3{ vertexArray[indexArray[i]].position + vertexArray[indexArray[i + 1]].position + vertexArray[indexArray[i + 2]].position } / 3.f;

		fnormal_lines.push_back({ fCenter, glm::vec3{0.0,1.f,0.f} });
		fnormal_lines.push_back({ fCenter + face_normals[i] * 0.02f, glm::vec3{ 0.f,1.f,0.f } });
	}
}

void Mesh::ResetVertices()
{
	//center
	glm::vec3 center{ 0.f };
	for (size_t i = 0; i < nrOfVertices; ++i)
	{
		center += vertexArray[i].position;
	}
	center /= nrOfVertices;
	for (size_t i = 0; i < nrOfVertices; ++i)
	{
		vertexArray[i].position -= center;
	}

	//resize
	float max_dis = 0;
	int max_ind = 0;
	glm::vec3 origin{ 0.f };

	for (size_t i = 0; i < nrOfVertices; ++i)
	{
		float dis = glm::distance(origin, vertexArray[i].position);
		if (max_dis < dis)
		{
			max_dis = dis;
			max_ind = i;
		}
	}

	glm::vec3 max_ver = vertexArray[max_ind].position;
	for (size_t i = 0; i < nrOfVertices; ++i)
	{
		vertexArray[i].position /= max_dis;
	}
}

void Mesh::SetOrbitPoints()
{
	float div = 120;
	float dis = 2.f;
	float step = 2.f * PI / div;

	for (int i = 0; i < div; i+=2)
	{
		orbit_lines.push_back({ glm::vec3{ dis * cosf(i * step), dis * sinf(i * step), 0.f }, glm::vec3{0.f,1.f,0.f} });
		if (i == div - 1)
			orbit_lines.push_back({ glm::vec3{ dis * cosf(0), dis * sinf(0),0.f}, glm::vec3{0.f,1.f,0.f} });
		else
			orbit_lines.push_back({ glm::vec3{ dis * cosf((i + 1) * step),dis * sinf((i + 1) * step),0.f }, glm::vec3{0.f,1.f,0.f} });
	}
}

void Mesh::InitVAO()
{
	//Create VAO
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Gen VBO and bind and send data
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nrOfVertices * sizeof(Vertex), vertexArray.data(), GL_STATIC_DRAW);

	//Gen EBO and bind and send data
	if (nrOfIndices > 0)
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nrOfIndices * sizeof(GLuint), indexArray.data(), GL_STATIC_DRAW);
	}

	//Set vertex attribute pointers and enable (input assembly)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(3);

	//Bind VAO 0
	glBindVertexArray(0);

	///////////////////////////////////
	glCreateVertexArrays(1, &VAO_nr);
	glBindVertexArray(VAO_nr);

	glGenBuffers(1, &VBO_nr);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_nr);
	glBufferData(GL_ARRAY_BUFFER, normal_lines.size() * sizeof(Point), normal_lines.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)offsetof(Point, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)offsetof(Point, color));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	/////////////////////////////////
	glCreateVertexArrays(1, &VAO_fnr);
	glBindVertexArray(VAO_fnr);

	glGenBuffers(1, &VBO_fnr);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_fnr);
	glBufferData(GL_ARRAY_BUFFER, fnormal_lines.size() * sizeof(Point), fnormal_lines.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)offsetof(Point, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)offsetof(Point, color));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	/////////////////////////////////
	glCreateVertexArrays(1, &VAO_or);
	glBindVertexArray(VAO_or);

	glGenBuffers(1, &VBO_or);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_or);
	glBufferData(GL_ARRAY_BUFFER, orbit_lines.size() * sizeof(Point), orbit_lines.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)offsetof(Point, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)offsetof(Point, color));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

