/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: Basic camera class cpp file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_3
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#include "Camera.h"

#include <iostream>

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
	case UP:
		position += up * movementSpeed * dt;
		break;
	case DOWN:
		position -= up * movementSpeed * dt;
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

void Camera::AddRotation(float pitch_, float yaw_, float roll_)
{
	pitch += pitch_;
	yaw += yaw_;
	roll += roll_;

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

void Camera::SetCameraDirPos(FACING_DIR dir, glm::vec3 pos)
{
	position = pos;
	
	switch (dir)
	{
	case CAM_LEFT:
	{
		pitch = 0.f;
		yaw = 180.f;
		break;
	}
	case CAM_RIGHT:
	{
		pitch = 0.f;
		yaw = 0.f;
		break;
	}
	case CAM_BOTTOM:
	{
		pitch = -90.f;
		yaw = 0.f;
		break;
	}
	case CAM_TOP:
	{
		pitch = 90.f;
		yaw = 0.f;
		break;
	}
	case CAM_BACK:
	{
		pitch = 0.f;
		yaw = 90.f;
		break;
	}
	case CAM_FRONT:
	{
		pitch = 0.f;
		yaw = -90.f;
		break;
	}
	}
}

void Camera::UpdateCameraVectors()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y =							sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, worlUp));
	up = glm::normalize(glm::cross(right, front));
}