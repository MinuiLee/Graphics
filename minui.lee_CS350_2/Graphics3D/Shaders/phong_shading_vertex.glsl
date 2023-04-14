/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phong_shading_vertex.glsl
Purpose: Vertex shader source
Language: glsl
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#version 450

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;

out vec2 vs_texcoord;

void main()
{
	vs_texcoord = vertex_texcoord;//vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);
	gl_Position = vec4(vertex_position, 1.0);
}