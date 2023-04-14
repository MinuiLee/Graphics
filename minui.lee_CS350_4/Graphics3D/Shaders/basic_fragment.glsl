/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: basic_fragment.glsl
Purpose: Fragment shader source
Language: glsl
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#version 450

in vec3 vs_position;

out vec4 fs_color;

uniform vec3 line_color;

void main()
{
	fs_color = vec4(line_color, 1.f);
}