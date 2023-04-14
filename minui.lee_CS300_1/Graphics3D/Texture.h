/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Texture.h
Purpose: Texture class which loads images
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#pragma once

#include <iostream>
#include <glew.h>
#include <SOIL2.h>

class Texture
{
public:
	Texture(const char* fileName, GLenum type_)
		:type(type_)
	{
		loadFromFile(fileName);
	}

	~Texture()
	{
		glDeleteTextures(1, &id);
	}

	GLuint getID() const { return id; }

	void bind(const GLint texture_unit)
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glBindTexture(type, id);
	}

	void unbind()
	{
		glActiveTexture(0);
		glBindTexture(type, 0);
	}

	void loadFromFile(const char * fileName)
	{
		if (id)
		{
			glDeleteTextures(1, &id);
		}

		unsigned char* image = SOIL_load_image(fileName, &width, &height, nullptr, SOIL_LOAD_RGBA);

		glGenTextures(1, &id);
		glBindTexture(type, id);

		glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if (image)
		{
			glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(type);
		}
		else
		{
			std::cout << "ERROR::LOAD IMAGE FAILED: " << fileName << " \n";
		}

		glActiveTexture(0);
		glBindTexture(type, 0);
		SOIL_free_image_data(image);
	}
	
private:
	GLuint id = 0;
	int width;
	int height;
	unsigned int type;
};