/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Material.h
Purpose: Material class header that handles light properties
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#pragma once

#include <glew.h>
#include "Shader.h"

class Material
{
public:
	Material(float shininess_, GLint diffuseTex_, GLint specularTex_)
			: shininess(shininess_), diffuseTex(diffuseTex_), specularTex(specularTex_)
	{}

	~Material(){}

	void sendToShader(Shader & program)
	{
		program.Set1f(shininess, "material.shininess");
		program.Set1i(diffuseTex, "material.diffuseTex");
		program.Set1i(specularTex, "material.specularTex");
	}

	void sendShiniesssToShader(Shader& program)
	{
		program.Set1f(shininess, "material.shininess");
	}

	void sendDiffSpecToShader(Shader& program)
	{
		program.Set1i(diffuseTex, "material.diffuseTex");
		program.Set1i(specularTex, "material.specularTex");
	}

	float* GetShininess() { return &shininess; }
	
private:
	float shininess;
	GLint diffuseTex;
	GLint specularTex;
};