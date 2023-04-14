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
#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include "Shader.h"

enum class LIGHTTYPE
{
	DIRECTIONAL = 0,
	POINT,
	SPOTLIGHT,
	END
};

class Light
{
public:
	Light(glm::vec3 position_, glm::vec3 direction_, float ambient_, float diffuse_, float specular_, float innerCutOff_, float outerCutOff_, LIGHTTYPE type_)
		: position(position_), direction(direction_), ambient(ambient_), diffuse(diffuse_), specular(specular_), constant(1.f),
		linear(0.09f), quadratic(0.032f), innerCutOff(innerCutOff_), outerCutOff(outerCutOff_), fallOff(1.f), type(type_)
	{
		isDirectional = false;
		isSpotlight = false;
		isPoint = false;
		
		if (type == LIGHTTYPE::DIRECTIONAL)
			isDirectional = true;
		else if (type == LIGHTTYPE::POINT)
			isPoint = true;
		else
			isSpotlight = true;

		ambient_f[0] = ambient[0] ; ambient_f[1] = ambient[1] ;  ambient_f[2] = ambient[2] ;
		diffuse_f[0] = diffuse[0] ; diffuse_f[1] = diffuse[1] ;  diffuse_f[2] = diffuse[2] ;
		specular_f[0] = specular[0] ; specular_f[1] = specular[1] ;  specular_f[2] = specular[2] ;
		position_f[0] = position[0]; position_f[1] = position[1]; position_f[2] = position[2];
		direction_f[0] = direction[0]; direction_f[1] = direction[1]; direction_f[2] = direction[2];
	}

	~Light() {}

	void sendToShader(Shader& program, int num)
	{
		std::string s = "lights[" + std::to_string(num) + "].";
		program.SetVec3f(position, s + "position");
		program.SetVec3f(direction, s + "direction");
		
		program.SetVec3f(ambient, s + "ambient");
		program.SetVec3f(diffuse, s + "diffuse");
		program.SetVec3f(specular, s + "specular");
		
		program.Set1f(constant, s + "constant");
		program.Set1f(linear, s + "linear");
		program.Set1f(quadratic, s + "quadratic");
		
		program.Set1f(glm::cos(glm::radians(innerCutOff)), s + "innerCutOff");
		program.Set1f(glm::cos(glm::radians(outerCutOff)), s + "outerCutOff");
		program.Set1f(fallOff, s + "fallOff");
		
		program.Set1i(static_cast<int>(type), s + "type");
	}

	void UpdateLightType(LIGHTTYPE type_)
	{
		type = type_;
		isDirectional = false;
		isSpotlight = false;
		isPoint = false;
		if (type_ == LIGHTTYPE::DIRECTIONAL)
			isDirectional = true;
		else if (type_ == LIGHTTYPE::POINT)
			isPoint = true;
		else
			isSpotlight = true;
	}
	
	void SetPosition(glm::vec3 position_) { position = position_; position_f[0] = position[0]; position_f[1] = position[1]; position_f[2] = position[2];}
	void SetDirection(glm::vec3 direction_) { direction = direction_; direction_f[0] = direction[0]; direction_f[1] = direction[1]; direction_f[2] = direction[2];}
	glm::vec3 GetPosition() { return position; }
	float* GetPositionFloat() { return position_f; }
	float* GetDirectionFloat() { return direction_f; }
	void UpdatePosition(){ position[0] = position_f[0]; position[1] = position_f[1]; position[2] = position_f[2];	}
	void UpdateDirection(){ direction[0] = direction_f[0]; direction[1] = direction_f[1]; direction[2] = direction_f[2]; }
	
	glm::vec3& GetAmbient() { return ambient; }
	glm::vec3& GetDiffuse() { return diffuse; }
	glm::vec3& GetSpecular() { return specular; }
	void UpdateAmbient() {ambient[0] = ambient_f[0] ; ambient[1] = ambient_f[1] ;  ambient[2] = ambient_f[2] ;}
	void UpdateDiffuse() { diffuse[0] = diffuse_f[0] ; diffuse[1] = diffuse_f[1] ;  diffuse[2] = diffuse_f[2] ; }
	void UpdateSpecular() { specular[0] = specular_f[0] ; specular[1] = specular_f[1] ;  specular[2] = specular_f[2] ; }
	void UpdateDiffuseF() { diffuse_f[0] = diffuse[0]; diffuse_f[1] = diffuse[1];  diffuse_f[2] = diffuse[2]; }
	float* GetAmbientFloat() { return ambient_f; }
	float* GetDiffuseFloat() { return diffuse_f; }
	float* GetSpecularFloat() { return specular_f; }

	float& GetInnerAngle() { return innerCutOff; }
	float& GetOuterAngle() { return outerCutOff; }
	float& GetFallOff() { return fallOff; }
	
	LIGHTTYPE& GetLightType() { return type; }

	bool& GetIsDirectional() { return isDirectional; }
	bool& GetIsPoint() { return isPoint; }
	bool& GetIsSpotlight() { return isSpotlight; }
	
private:
	glm::vec3 position;
	glm::vec3 direction;
	
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
	
	float innerCutOff;
	float outerCutOff;
	float fallOff;

	LIGHTTYPE type;

	bool isDirectional;
	bool isPoint;
	bool isSpotlight;

	float ambient_f[3];
	float diffuse_f[3];
	float specular_f[3];
	float position_f[3];
	float direction_f[3];
}; 

