/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: g_buffer_vertex.glsl
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

out vec3 vs_position;
out vec2 vs_texcoord;
out vec3 vs_normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
	vec4 worldPos = ModelMatrix * vec4(vertex_position, 1.0);
	vs_position = worldPos.xyz;
	vs_texcoord = vertex_texcoord;

	mat3 normalMattrix = transpose(inverse(mat3(ModelMatrix)));
	vs_normal = normalMattrix * vertex_normal;

	gl_Position = ProjectionMatrix * ViewMatrix * worldPos;
}