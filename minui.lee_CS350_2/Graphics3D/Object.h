/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.h
Purpose: Object class header which contains a mesh and transformation properties
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "Light.h"

class Object
{
public:
	Object(glm::vec3 position, Material* material, std::vector<Light*> lights, Texture* orTexDif, Texture* orTexSpec);
	~Object() {}

	void update() {}
	void Render(Shader* shader, Mesh* mesh, GLenum mode = GL_TRIANGLES);
	void RenderNormal(Shader* shader, Mesh* mesh);
	void RenderFaceNormal(Shader* shader, Mesh* mesh);
	void RenderObjectInNDC(Shader* shader, Mesh* mesh, GLenum mode = GL_TRIANGLES);

	void UpdateModelMatrix();
	void Move(const glm::vec3 position_) { position += position_; }
	void Rotate(const glm::vec3 rotation_) { rotation += rotation_; }
	void Scale(const glm::vec3 scale_) { scale += scale_; }
	void SetPosition(const glm::vec3 position_) { position = position_; }
	void SetOrigin(const glm::vec3 origin_) { origin = origin_; }
	void SetRotation(const glm::vec3 rotation_) { rotation = rotation_; }
	void SetScale(const glm::vec3 scale_) { scale = scale_; }
	void SetColor(glm::vec3 color_) { color = color_; }
	void SetLight(std::vector<Light*> lights_) { lights = lights_; }
	
	glm::vec3 GetPosition() { return ModelMatrix * glm::vec4(position, 1.f); }
	glm::vec3 GetRawPosition() { return glm::vec4(position, 1.f); }
	glm::vec3* GetRotation() { return &rotation; }
	glm::vec3& SetColor() { return color; }
	bool& GetCanRotate() { return canRotate; }
	
	void MoveByAngle(float a, float dis)
	{
		angle += a;
		if (angle > (2.f * PI)) 
			angle = 0.f;
		SetPosition(glm::vec3{ dis * cosf(angle), 0.f, dis * sinf(angle) });
	}
	void SetAngle(float a) { angle = a; }
	
	void UpdateUniform(Shader* shader)
	{
		shader->SetMat4fv(ModelMatrix, "ModelMatrix");
		//shader->SetVec3f(color, "vs_color");
	}

public:
	float scaleFactor;

private:
	Material* material;
	std::vector<Light*> lights;
	Texture* overrideTextureDiffuse;
	Texture* overiideTextureSpecular;

	glm::vec3 position;
	glm::vec3 origin;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::vec3 color;

	float angle;
	bool canRotate = true;

	glm::mat4 ModelMatrix;
};