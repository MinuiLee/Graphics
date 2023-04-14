/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phong_lighting_vertex.glsl
Purpose: Vertex shader source
Language: glsl
Platform: Visual Studio 2019
Project: minui.lee_CS300_3
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#version 450
const float PI = 3.1415926535897932384626433832795;

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

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;

vec3 vs_position;
out vec3 vs_color;
vec2 vs_texcoord;
vec3 vs_normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform Material material;
uniform Light lights[16];
uniform vec3 cameraPos;
uniform int numLights;
uniform bool texMappingGPU; 
uniform bool useNormalForMapping;
uniform int mappingKind; // 0 Cylinderical | 1 Spherical | 2 Cube
uniform vec3 fogColor;
uniform vec3 globalAmbient;

float zNear = 0.1f;
float zFar = 100.f;

// Ambient
vec3 calculateAmbient(Light light)
{
	return light.ambient * texture(material.diffuseTex, vs_texcoord).rgb;
}

//Diffuse
vec3 calculateDiffuse(Light light, vec3 lightDir)
{
	float diffuse = max(dot(normalize(vs_normal), lightDir), 0.0);
	vec3 diffuseFinal =  light.diffuse * diffuse * texture(material.diffuseTex, vs_texcoord).rgb;

	return diffuseFinal;
}

// Speculate
vec3 calculateSpeculate(Light light, vec3 lightDir)
{
	vec3 nNormal = normalize(vs_normal);
	vec3 viewDir = normalize(cameraPos - vs_position);
	vec3 reflectDir = 2*nNormal*dot(nNormal,lightDir)-lightDir;
	float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);
	vec3 specular =  light.specular * spec * texture(material.specularTex, vs_texcoord).rgb;

	return specular;
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
	vec3 lightDir = normalize(light.position - vs_position);
	float dis    = length(light.position - vs_position);
    float attenuation = min(light.constant + light.linear * dis + light.quadratic * (dis * dis), 1.f);  

	vec3 ambient = calculateAmbient(light) * attenuation;
	vec3 diffuse =  calculateDiffuse(light, lightDir) * attenuation;
	vec3 specular =  calculateSpeculate(light, lightDir) * attenuation;

	return vec4(ambient, 1.f) + vec4(diffuse, 1.f) + vec4(specular, 1.f);
}

vec4 getSpotlight(Light light)
{
	vec3 lightDir = normalize(light.position - vs_position);

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.innerCutOff - light.outerCutOff);
    float intensity = pow(clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0), light.fallOff);

	float dis    = length(light.position - vs_position);
	float attenuation = min(light.constant + light.linear * dis + light.quadratic * (dis * dis), 1.f);    

	vec3 ambient = calculateAmbient(light) * attenuation;
	vec3 diffuse =  calculateDiffuse(light, lightDir) * attenuation * intensity;
	vec3 specular =  calculateSpeculate(light, lightDir) * attenuation * intensity;

	return vec4(ambient, 1.f) + vec4(diffuse, 1.f) + vec4(specular, 1.f);
}

vec2 CylindericalMapping(vec3 pos)
{
	float  theta = degrees(atan(pos.z, pos.x));
	theta += 180.f;

	return vec2(theta/360.f, (pos.y + 1.f) * 0.5f);
}

vec2 SphericalMapping(vec3 pos)
{
	float  theta = degrees(atan(pos.z, pos.x));
	theta += 180.f;
	
	float phi = degrees(acos(pos.y / length(pos)));

	return vec2(theta/360.f, 1.f - (phi/180.f));
}

vec2 CubeMapping(vec3 pos)
{
	vec3 absVec = abs(pos);
	vec2 uv = vec2(0.f);

	if (absVec.x >= absVec.y && absVec.x >= absVec.z)
	{
		(pos.x < 0.f) ? (uv.x = pos.z) : (uv.x = -pos.z);
		uv.y = pos.y;
	}
	else if (absVec.y >= absVec.x && absVec.y >= absVec.z)
	{
		(pos.y < 0.f) ? (uv.x = pos.z) : (uv.x = -pos.z);
		uv.y = pos.x;
	}
	else if (absVec.z >= absVec.x && absVec.z >= absVec.y)
	{
		(pos.z < 0.f) ? (uv.x = -pos.x) : (uv.x = pos.x);
		uv.y = pos.y;
	}

	uv.x = 0.5f * (uv.x + 1.0f);
	uv.y = 0.5f * (uv.y + 1.0f);
	return uv;
}

void main()
{
	vs_position = vec4(ModelMatrix * vec4(vertex_position, 1.f)).xyz;
	vs_normal = normalize(mat3(transpose(inverse(ModelMatrix))) * vertex_normal);
	vs_color = vec3(0.f);

	if(texMappingGPU)
	{
		if(useNormalForMapping)
		{
			if(mappingKind == 0)
			{
				vs_texcoord = CylindericalMapping(vertex_normal);
			}
			else if(mappingKind == 1)
			{
				vs_texcoord = SphericalMapping(vertex_normal);
			}
			else //Cube
			{
				vs_texcoord = CubeMapping(vertex_normal);
			}
		}
		else
		{
			if(mappingKind == 0)
			{
				vs_texcoord = CylindericalMapping(vertex_position);
			}
			else if(mappingKind == 1)
			{
				vs_texcoord = SphericalMapping(vertex_position);
			}
			else //Cube
			{
				vs_texcoord = CubeMapping(vertex_position);
			}
		}
	}
	else
	{
		vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);
	}

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);


	for(int i = 0; i < numLights; i++)
	{
		if(lights[i].type == 0) //directional
			vs_color += getDirectionalLight(lights[i]).xyz;
		else if(lights[i].type == 1) //point
			vs_color += getPointLight(lights[i]).xyz;
		else if(lights[i].type == 2) //spotlight
			vs_color += getSpotlight(lights[i]).xyz;
	}

	for(int i = 0; i < numLights; i++)
	{
		vs_color += globalAmbient * lights[i].ambient;
	}

	float S = (zFar - distance(vs_position, cameraPos))/(zFar - zNear); 
	vs_color = S*vs_color + (1.f-S)*fogColor;
}