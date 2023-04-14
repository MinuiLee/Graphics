/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phong_shading_fragment.glsl
Purpose: Fragment shader source
Language: glsl
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#version 450
const float PI = 3.14159265;

struct Material
{
	float shininess;
	sampler2D diffuseTex;
	sampler2D specularTex;
};

struct Light
{
	// 0 == directional | 1 == Point | 2 == Spotlight
	int type;

	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
    float linear;
    float quadratic;

	float innerCutOff;
    float outerCutOff;
    float fallOff;
};

uniform Material material;
uniform Light lights[16];
uniform vec3 cameraPos;
uniform int numLights;
uniform vec3 fogColor;
uniform vec3 globalAmbient;

out vec4 fs_color;

float zNear = 0.1f;
float zFar = 100.f;

//-------------------- CS350 -----------------------//

in vec2 vs_texcoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

vec3 position;
vec3 normal;
vec3 diffuse;
float specular;


// Ambient
vec3 calculateAmbient(Light light)
{
	return light.ambient;// * texture(material.diffuseTex, uv).rgb;
}

//Diffuse
vec3 calculateDiffuse(Light light, vec3 lightDir)
{
	float diffuse_ = max(dot(normalize(normal), lightDir), 0.0);
	vec3 diffuseFinal = light.diffuse * diffuse_ * diffuse;

	return diffuseFinal;
}

// Speculate
vec3 calculateSpeculate(Light light, vec3 lightDir)
{
	vec3 nNormal = normalize(normal);
	vec3 viewDir = normalize(cameraPos - position);
	vec3 reflectDir = 2*nNormal*dot(nNormal,lightDir)-lightDir;
	float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);
	vec3 specular_ =  light.specular * spec * specular;

	return specular_;
}

// Final light
vec4 getDirectionalLight(Light light)
{
	vec3 lightDir = normalize(-light.direction);
	vec3 ambient = calculateAmbient(light);
	vec3 diffuse =  calculateDiffuse(light, lightDir);
	vec3 specular =  calculateSpeculate(light, lightDir);

	return vec4(ambient, 1.f) + vec4(diffuse, 1.f) + vec4(specular, 1.f);
}

vec4 getPointLight(Light light)
{
	vec3 lightDir = normalize(light.position - position);
	float dis    = length(light.position - position) * 50.f;
    float attenuation = 1.f / (light.constant + light.linear * dis + light.quadratic * (dis * dis));  

	vec3 ambient_ = calculateAmbient(light) * attenuation;
	vec3 diffuse_ =  calculateDiffuse(light, lightDir) * attenuation;
	vec3 specular_ =  calculateSpeculate(light, lightDir) * attenuation;

	return vec4(ambient_, 1.f) + vec4(diffuse_, 1.f) + vec4(specular_, 1.f);
}

vec4 getSpotlight(Light light)
{
	vec3 lightDir = normalize(light.position - position);

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.innerCutOff - light.outerCutOff);
    float intensity = pow(clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0), light.fallOff);

	float dis    = length(light.position - position);
	float attenuation = 1.f / (light.constant + light.linear * dis + light.quadratic * (dis * dis));    

	vec3 ambient_ = calculateAmbient(light) * attenuation;
	vec3 diffuse_ =  calculateDiffuse(light, lightDir) * attenuation * intensity;
	vec3 specular_ =  calculateSpeculate(light, lightDir) * attenuation * intensity;

	return vec4(ambient_, 1.f) + vec4(diffuse_, 1.f) + vec4(specular_, 1.f);
}

void main()
{
	position = texture(gPosition, vs_texcoord).rgb;
	normal = texture(gNormal, vs_texcoord).rgb;
	diffuse = texture(gAlbedoSpec, vs_texcoord).rgb;
	specular = texture(gAlbedoSpec, vs_texcoord).a;

	for(int i = 0; i < numLights; i++)
	{
		if(lights[i].type == 0) //directional
			fs_color += getDirectionalLight(lights[i]);
		else if(lights[i].type == 1) //point
			fs_color += getPointLight(lights[i]);
		else if(lights[i].type == 2) //spotlight
			fs_color += getSpotlight(lights[i]);
	}

	for(int i = 0; i < numLights; i++)
	{
		fs_color += vec4(globalAmbient * lights[i].ambient, 1.f);
	}

	float S = (zFar - distance(position, cameraPos))/(zFar - zNear); 
	fs_color = S*fs_color + vec4((1.f-S)*fogColor, 1.f);
}