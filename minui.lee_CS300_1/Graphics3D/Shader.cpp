/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.cpp
Purpose: Shader class cpp file which load and link shaders
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#include "Shader.h"

Shader::Shader(const int versionMajor_, const int versionMinor_, std::string vertexFile, std::string fragmentFile,
               std::string geometryFile): versionMajor(versionMajor_), versionMinor(versionMinor_)
{
	GLuint vertexShader = 0;
	GLuint geometryShader = 0;
	GLuint fragmentShader = 0;

	vertexShader = LoadShader(GL_VERTEX_SHADER, vertexFile);

	if (geometryFile != "")
	{
		geometryShader = LoadShader(GL_GEOMETRY_SHADER, geometryFile);
	}

	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentFile);

	LinkProgram(vertexShader, geometryShader, fragmentShader);

	//End
	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

	glUseProgram(0);
}

std::string Shader::LoadShaderSource(std::string filename)
{
	std::string tmp = "";
	std::string src = "";

	std::ifstream in_file;

	//vertex
	in_file.open(filename);
	if (in_file.is_open())
	{
		while (std::getline(in_file, tmp))
			src += tmp + "\n";
	}
	else
	{
		std::cout << "ERROR::COULD NOT OPEN SHADER FILE: " << filename << "\n";
	}

	in_file.close();

	std::string version =
		std::to_string(versionMajor) +
		std::to_string(versionMinor) +
		"0";

	src.replace(src.find("#version"), 12, "#version " + version);

	return src;
}

GLuint Shader::LoadShader(GLenum type, std::string filename)
{
	char infoLog[512];
	GLint success;

	GLuint shader = glCreateShader(type);
	std::string str_src = LoadShaderSource(filename);
	const GLchar* src = str_src.c_str();
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::COULD NOT COMPILE SHADER: " << filename << "\n";
		std::cout << infoLog << "\n";
	}

	return shader;
}

void Shader::LinkProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader)
{
	char infoLog[512];
	GLint success;

	id = glCreateProgram();

	glAttachShader(id, vertexShader);

	if (geometryShader)
	{
		glAttachShader(id, geometryShader);
	}

	glAttachShader(id, fragmentShader);

	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::COULD NOT LINK PROGRAM\n";
		std::cout << infoLog << "\n";
	}
}
