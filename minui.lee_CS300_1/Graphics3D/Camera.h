/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.h
Purpose: Basic camera class header
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#pragma once

#include <glew.h>
#include <glm.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

enum direction
{
	FORWARD = 0,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp);
	~Camera(){}

	const glm::mat4 GetViewMatrix()
	{
		UpdateCameraVectors();
		viewMatrix = glm::lookAt(position, position + front, up);
		return viewMatrix;
	};
	const glm::vec3 GetPosition() const {return position;}

	void Move(const float& dt, const int direction);
	void UpdateInput(const float& dt, const int direction, const double& offsetX, const double& offsetY);
	
private:
	glm::mat4 viewMatrix;

	GLfloat movementSpeed;
	GLfloat sensitivity;;
	
	glm::vec3 worlUp;
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	GLfloat pitch;
	GLfloat yaw;
	GLfloat roll;
	
	void UpdateCameraVectors();
};
