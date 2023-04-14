/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Material.h
Purpose: Material class header that handles light properties
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#pragma once

#include <glew.h>
#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include "Shader.h"

class Material
{
public:
	Material(glm::vec3 ambient_, glm::vec3 diffuse_, glm::vec3 specular_, float shininess_,
	GLint diffuseTex_, GLint specularTex_)
			: ambient(ambient_), diffuse(diffuse_), specular(specular_), shininess(shininess_),
			diffuseTex(diffuseTex_), specularTex(specularTex_),ambient_1(0.f), diffuse_1(0.f),specular_1(0.f)
	{}

	~Material(){}

	void sendToShader(Shader & program)
	{
		program.SetVec3f(ambient, "material.ambient");
		program.SetVec3f(diffuse, "material.diffuse");
		program.SetVec3f(specular, "material.specular");
		program.Set1f(shininess, "material.shininess");
		program.Set1i(diffuseTex, "material.diffuseTex");
		program.Set1i(specularTex, "material.specularTex");
	}

	glm::vec3& GetAmbient() { return ambient; }
	glm::vec3& GetDiffuse() { return diffuse; }
	glm::vec3& GetSpecular() { return specular; }
	float* GetShininess() { return &shininess; }
	float* GetAmbient1f() { return &ambient_1; }
	float* GetDiffuse1f() { return &diffuse_1; }
	float* GetSpecular1f() { return &specular_1; }

	void ResetAmbient() { ambient = glm::vec3(ambient_1); }
	void ResetDiffuse() { diffuse = glm::vec3(diffuse_1); }
	void ResetSpecular() { specular = glm::vec3(specular_1); }
	
private:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	GLint diffuseTex;
	GLint specularTex;

	float ambient_1;
	float diffuse_1;
	float specular_1;
};