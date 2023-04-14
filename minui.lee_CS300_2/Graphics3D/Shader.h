/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.h
Purpose: Shader class header which load and link shaders
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#pragma once

#include <iostream>
#include <fstream>
#include <string>

//include glew before glfw
#include <glew.h>

#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <gtc/type_ptr.hpp>


class Shader
{
public:
	Shader(const int versionMajor, const int versionMinor, std::string vertexFile, std::string fragmentFile,
	       std::string geometryFile = "");
	~Shader(){glDeleteProgram(id);}

	void Use(){glUseProgram(id);}
	void Unuse(){glUseProgram(0);}

	void Set1i(GLint value, std::string name)
	{
		Use();
		glUniform1i(glGetUniformLocation(id, name.c_str()) , value);
		Unuse();
	}
	
	void Set1f(GLfloat value, std::string name)
	{
		Use();
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		Unuse();
	}
	
	void SetVec2f(glm::fvec2 value, const GLchar* name)
	{
		Use();
		glUniform2fv(glGetUniformLocation(id, name), 1, glm::value_ptr(value));
		Unuse();
	}
	
	void SetVec3f(glm::fvec3 value, std::string name)
	{
		Use();
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
		Unuse();
	}

	void SetVec3f(float value[3], std::string name)
	{
		Use();
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, value);
		Unuse();
	}
	
	void SetVec4f(glm::fvec4 value, const GLchar* name)
	{
		Use();
		glUniform4fv(glGetUniformLocation(id, name), 1, glm::value_ptr(value));
		Unuse();
	}

	void SetMat3fv(glm::mat3 value, const GLchar* name, GLboolean transpose = GL_FALSE)
	{
		Use();
		glUniformMatrix3fv(glGetUniformLocation(id, name), 1, transpose, glm::value_ptr(value));
		Unuse();
	}
	
	void SetMat4fv(glm::mat4 value, const GLchar* name, GLboolean transpose = GL_FALSE)
	{
		Use();
		glUniformMatrix4fv(glGetUniformLocation(id, name), 1, transpose, glm::value_ptr(value));
		Unuse();
	}
private:
	GLuint id;
	const int versionMajor;
	const int versionMinor;

	std::string LoadShaderSource(std::string filename);
	GLuint LoadShader(GLenum type, std::string filename);
	void LinkProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader);
};