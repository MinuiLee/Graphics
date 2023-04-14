/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: basic_vertex.glsl
Purpose: Vertex shader source
Language: glsl
Platform: Visual Studio 2019
Project: minui.lee_CS300_3
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#version 450

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;

out vec3 vs_color;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
	vs_color = vertex_color;

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
}