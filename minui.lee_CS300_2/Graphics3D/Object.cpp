/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.cpp
Purpose: Object class cpp file which contains a mesh and transformation properties
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/


#include "Object.h"

Object::Object(glm::vec3 position_, Material* material_, std::vector<Light*> lights_, Texture* orTexDif, Texture* orTexSpec, std::string meshName_)
{
	position = position_;
	origin = glm::vec3(0.f);
	rotation = glm::vec3(0.f);
	scale = glm::vec3(1.f);
	color = glm::vec3(1.f);
	material = material_;
	lights = lights_;
	overiideTextureSpecular = orTexSpec;
	overrideTextureDiffuse = orTexDif;
	meshName = meshName_;
	angle = 0.f;
	canRotate = true;
	UpdateModelMatrix();
}

void Object::Render(Shader* shader, Mesh* mesh, GLenum mode)
{
	UpdateModelMatrix();
	UpdateUniform(shader);
	material->sendToShader(*shader);
	
	int size = lights.size();
	for (int i = 0; i < size; ++i)
		lights[i]->sendToShader(*shader, i);
	shader->Use();

	if(overrideTextureDiffuse)
		overrideTextureDiffuse->bind(0);
	if(overiideTextureSpecular)
		overiideTextureSpecular->bind(1);

	mesh->Render(shader, mode);
}

void Object::RenderNormal(Shader* shader, Mesh* mesh)
{
	UpdateModelMatrix();
	UpdateUniform(shader);
	material->sendToShader(*shader);
	shader->Use();
	mesh->RenderNormal(shader);
}

void Object::RenderFaceNormal(Shader* shader, Mesh* mesh)
{
	UpdateModelMatrix();
	UpdateUniform(shader);
	material->sendToShader(*shader);
	shader->Use();
	mesh->RenderFaceNormal(shader);
}

void Object::RenderOrbit(Shader* shader, Mesh* mesh)
{
	UpdateModelMatrix();
	UpdateUniform(shader);
	material->sendToShader(*shader);
	shader->Use();
	mesh->RenderOrbit(shader);
}

void Object::UpdateModelMatrix()
{
	ModelMatrix = glm::mat4(1.f);
	ModelMatrix = glm::translate(ModelMatrix, position);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
	ModelMatrix = glm::scale(ModelMatrix, scale);
}
