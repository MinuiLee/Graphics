/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: fragment_line.glsl
Purpose: Fragment shader source for line drawing
Language: glsl
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#version 450

in vec3 vs_position;
in vec3 vs_color;

out vec4 fs_color;

void main()
{
	fs_color = vec4(vs_color, 1.f);
}