/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: Mesh class header which contains mesh properties such as vertices
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#pragma once

#include <vector>

#include "Shader.h"
#include "Primitives.h"

enum class TEXTURE_MAPPING
{
	CYLINDERICAL = 0,
	SPHERICAL,
	CUBE
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertexArray,std::vector<GLuint> indexArray);
	Mesh(Primitive* primitive);
	Mesh(std::string objFile);
	Mesh(const Mesh& obj);
	Mesh();
	~Mesh();
	
	void Update(){}

	void Render(Shader* shader, GLenum mode = GL_TRIANGLES);
	void RenderNormal(Shader* shader);
	void RenderFaceNormal(Shader* shader);
	void ChangeMappingKind(TEXTURE_MAPPING kind, bool useNormal);

	glm::vec3 GetMin() { return min; }
	glm::vec3 GetMax() { return max; }

	std::vector<Vertex> vertexArray;
	std::vector<GLuint> indexArray;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
private:
	unsigned nrOfVertices;
	unsigned nrOfIndices;
	std::vector<Point> normal_lines;
	std::vector<Point> fnormal_lines;
	std::vector<Point> orbit_lines; 
	std::vector<glm::vec3> face_normals;

	glm::vec3 min;
	glm::vec3 max;

	GLuint VAO_nr;
	GLuint VBO_nr;
	GLuint VAO_fnr;
	GLuint VBO_fnr;

	GLuint VAO_or;
	GLuint VBO_or;

	void InitVAO();
	void SetNormals();
	void ResetVertices();
	void CalculateSphericalTexCoord(bool useNormal);
	void CalculateCylindericalTexCoord(bool useNormal);
	void CalculateCubeTexCoord(bool useNormal);
};