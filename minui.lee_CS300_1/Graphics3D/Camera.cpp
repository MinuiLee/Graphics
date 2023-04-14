/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: Basic camera class cpp file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#include "Camera.h"

Camera::Camera(glm::vec3 position_, glm::vec3 direction_, glm::vec3 worldUp_)
{
	viewMatrix = glm::mat4(1.f);

	movementSpeed = 3.f;
	sensitivity = 5.f;

	worlUp = worldUp_;
	position = position_;
	right = glm::vec3(0.f);
	up = worldUp_;

	pitch = 0.f;
	yaw = -90.f;
	roll = 0.f;

	UpdateCameraVectors();
}

void Camera::Move(const float& dt, const int direction)
{
	switch (direction)
	{
	case FORWARD:
		position += front * movementSpeed * dt;
		break;
	case BACKWARD:
		position -= front * movementSpeed * dt;
		break;
	case LEFT:
		position -= right * movementSpeed * dt;
		break;
	case RIGHT:
		position += right * movementSpeed * dt;
		break;
	default: break;
	}
}

void Camera::UpdateInput(const float& dt, const int direction, const double& offsetX, const double& offsetY)
{
	//Update pitch yaw roll
	pitch += static_cast<GLfloat>(offsetY) * dt * sensitivity;
	yaw += static_cast<GLfloat>(offsetX) * dt * sensitivity;

	if (pitch > 80.f)
	{
		pitch = 80.f;
	}
	else if (pitch < -80.f)
	{
		pitch = -80.f;
	}

	if (yaw > 360.f || yaw < -360.f)
	{
		yaw = 0.f;
	}
}

void Camera::UpdateCameraVectors()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, worlUp));
	up = glm::normalize(glm::cross(right, front));
}
