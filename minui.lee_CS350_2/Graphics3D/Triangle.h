/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Triangle.h
Purpose: Triangle primitive class h file
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

class Triangle
{
public:
	Triangle() {}
	
	void Draw(Shader* shader, glm::vec3 color);
	bool Initialize(glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3);
public:
	glm::vec3 v1 = glm::vec3(0.f);
	glm::vec3 v2 = glm::vec3(0.f);
	glm::vec3 v3 = glm::vec3(0.f);

private:
	void InitVAO();
private:
	std::vector<glm::vec3> points;
	glm::mat4 modelmatrix = glm::mat4(1.f);
	GLuint VAO = 0, VBO = 0;
	unsigned nrOfPoints = 0;
};