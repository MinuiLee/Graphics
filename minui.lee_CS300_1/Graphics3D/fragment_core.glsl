/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: fragment_core.glsl
Purpose: Fragment shader source
Language: glsl
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#version 450

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D diffuseTex;
	sampler2D specularTex;
};

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;

uniform Material material;
uniform vec3 lightPos;
uniform vec3 cameraPos;

vec3 calculateAmbient(Material material)
{
	return material.ambient;
}

vec3 calculateDiffuse(Material material, vec3 vs_position, vec3 vs_normal, vec3 lightPos0)
{
	vec3 posToLightDirVec = normalize(lightPos - vs_position);
	float diffuse = clamp(dot(posToLightDirVec, vs_normal), 0, 1);
	vec3 diffuseFinal =  material.diffuse * diffuse;

	return diffuseFinal;
}

vec3 calculateSpeculate(Material material, vec3 vs_position, vec3 vs_normal, vec3 lightPos, vec3 cameraPos0)
{
	vec3 lightToPosDirVec = normalize(vs_position - lightPos);
	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal)));
	vec3 posToViewDirVec = normalize(cameraPos - vs_position);
	float spec = pow(max(dot(posToViewDirVec, reflectDirVec), 0), material.shininess);
	vec3 specular =  material.specular * spec;// * texture(material.specularTex, vs_texcoord).rgb;

	return specular;
}

void main()
{
	//fs_color = vec4(vs_color, 1.f);
	//fs_color = texture(texture0, vs_texcoord) * texture(texture1, vs_texcoord);
	
	vec3 ambient = calculateAmbient(material);
	vec3 diffuse =  calculateDiffuse(material, vs_position, vs_normal, lightPos);
	vec3 specular =  calculateSpeculate(material, vs_position, vs_normal, lightPos, cameraPos);

	fs_color = vec4(vs_color, 1.f) *
	//texture(material.diffuseTex, vs_texcoord) *
	(vec4(ambient, 1.f) + vec4(diffuse, 1.f) + vec4(specular, 1.f));
}