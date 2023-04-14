#pragma once

#include <iostream>

#include <glew.h>

#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <gtc/type_ptr.hpp>

#include "Object.h"

class GBuffer
{
public:
	GBuffer();
	~GBuffer();
	void Initialize(int width, int height);
	void Bind();
	void UnBind();
	void ActiveBindTexture();
	void CopyDepthBuffer();
private:

	unsigned int gBuffer;
	unsigned int gPosition;
	unsigned int gNormal;
	unsigned int gAlbedoSpec;

	int width;
	int height;
};