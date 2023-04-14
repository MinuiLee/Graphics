/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: g_buffer_fragment.glsl
Purpose: Fragment shader source
Language: glsl
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#version 450

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 vs_position;
in vec2 vs_texcoord;
in vec3 vs_normal;

struct Material
{
	float shininess;
	sampler2D diffuseTex;
	sampler2D specularTex;
};

uniform Material material;

void main()
{
    material.shininess;
    gPosition = vs_position;
    gNormal = normalize(vs_normal);
    gAlbedoSpec.rgb = texture(material.diffuseTex, vs_texcoord).rgb;
    gAlbedoSpec.a = texture(material.specularTex, vs_texcoord).r;
}