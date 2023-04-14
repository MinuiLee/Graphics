/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phong_shading_vertex.glsl
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

out vec3 vs_position;
out vec3 vs_color;
out vec2 vs_texcoord;
out vec3 vs_normal;
out vec3 raw_position;
out vec3 raw_normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

/////////////////////////////////////////

uniform vec3 cameraPos;
uniform vec3 Eta;
uniform float K_t;
out vec3 RefractR;
out vec3 RefractG;
out vec3 RefractB;

vec3 MyRefract(vec3 I, vec3 normal, float K)
{
	vec3 Refract = vec3(0.f);

	float N_dot_I = dot(normal, I);
	float part = 1.0f - K * K * (1.0f - N_dot_I * N_dot_I);

	if(part >= 0)
		Refract =  K * I - (K * (N_dot_I) + sqrt(part)) * normal;

	return Refract;
}

void main()
{
	vs_position = vec4(ModelMatrix * vec4(vertex_position, 1.f)).xyz;
	vs_normal = normalize(mat3(transpose(inverse(ModelMatrix))) * vertex_normal);
	vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);
	vs_color = vertex_color;

	raw_position = vertex_position;
	raw_normal = vertex_normal;

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);

	///////////////////////////////

	vec3 I = normalize(vs_position - cameraPos);
	
	RefractR = MyRefract(I, vs_normal, Eta[0]);
	RefractG = MyRefract(I, vs_normal, Eta[1]);
	RefractB = MyRefract(I, vs_normal, Eta[2]);

	RefractR = vec3(ModelMatrix * vec4(RefractR, 1.0));
	RefractG = vec3(ModelMatrix * vec4(RefractG, 1.0));
	RefractB = vec3(ModelMatrix * vec4(RefractB, 1.0));
}