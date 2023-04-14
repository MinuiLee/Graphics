/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Triangle.h
Purpose: Triangle primitive class cpp file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 10 Feb 2021
End Header --------------------------------------------------------*/

#include "Triangle.h"

void Triangle::Draw(Shader* shader, glm::vec3 color)
{
	shader->SetMat4fv(modelmatrix, "ModelMatrix");
	shader->SetVec3f(color, "line_color");

	shader->Use();

	//Bind VAO
	glBindVertexArray(VAO);

	//Draw
	glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(points.size()));

	//CleanUp
	glBindVertexArray(0);
	glUseProgram(0);
}

void Triangle::InitVAO()
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

bool Triangle::Initialize(glm::vec3 v1_, glm::vec3 v2_, glm::vec3 v3_)
{
	//float d1 = glm::distance(v1_, v2_);
	//float d2 = glm::distance(v1_, v3_);
	//float d3 = glm::distance(v2_, v3_);
	
	//if ((d1 + d2) < d3 ||(d1 + d3) < d2 || (d2 + d3) < d1) return false;

	v1 = v1_;
	v2 = v2_;
	v3 = v3_;

	points.clear();

	points.push_back(v1);
	points.push_back(v2);
	points.push_back(v3);

	nrOfPoints = static_cast<unsigned int>(points.size());
	InitVAO();

	return true;
}
