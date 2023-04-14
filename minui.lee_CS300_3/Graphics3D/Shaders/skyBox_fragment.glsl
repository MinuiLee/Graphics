/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: basic_fragment.glsl
Purpose: Fragment shader source
Language: glsl
Platform: Visual Studio 2019
Project: minui.lee_CS300_3
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#version 450

out vec4 fs_color;

in vec3 vs_position;
in vec2 vs_texcoord;
in vec3 vs_normal;

uniform sampler2D skyboxTex[6];

void main()
{
	vec3 abs = abs(vs_position);
	float max_val = max(max(abs.x, abs.y), abs.z);

	if(max_val == abs.x)
	{
		float neg = step(vs_position.x, 0.0);
		vec2 uv = vs_position.zy * vec2(mix(-1.0, 1.0, neg), -1);
		
		fs_color = texture(skyboxTex[int(neg)], (uv+1)/2);
	}
	else if(max_val == abs.y)
	{
		float neg = step(vs_position.y, 0.0);
		vec2 uv = vs_position.xz * vec2(1.0, mix(1.0, -1.0, neg));
		
		fs_color = texture(skyboxTex[2+int(neg)], (uv+1)/2);
	}
	else// if(max_val == abs.z)
	{
		float neg = step(vs_position.z, 0.0);
		vec2 uv = vs_position.xy * vec2(mix(1.0, -1.0, neg), -1);
		
		fs_color = texture(skyboxTex[4+int(neg)], (uv+1)/2);
	}
	
}