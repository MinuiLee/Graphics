/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: blinn_shading_fragment.glsl
Purpose: Fragment shader source
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

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;
in vec3 raw_position;
in vec3 raw_normal;

uniform Material material;
uniform Light lights[16];
uniform vec3 cameraPos;
uniform int numLights;
uniform bool texMappingGPU; 
uniform bool useNormalForMapping;
uniform int mappingKind; // 0 Cylinderical | 1 Spherical | 2 Cube
uniform vec3 fogColor;
uniform vec3 globalAmbient;

out vec4 fs_color;

vec2 uv;
float zNear = 0.1f;
float zFar = 100.f;

// Ambient
vec3 calculateAmbient(Light light)
{
	return light.ambient * texture(material.diffuseTex, uv).rgb;
}

//Diffuse
vec3 calculateDiffuse(Light light, vec3 lightDir)
{
	float diffuse = max(dot(normalize(vs_normal), lightDir), 0.0);
	vec3 diffuseFinal =  light.diffuse * diffuse * texture(material.diffuseTex, uv).rgb;

	return diffuseFinal;
}

// Speculate
vec3 calculateSpeculate(Light light, vec3 lightDir)
{
	vec3 viewDir = normalize(cameraPos - vs_position);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(vs_normal, halfwayDir), 0), material.shininess);
	vec3 specular =  light.specular * spec * texture(material.specularTex, uv).rgb;

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
	if(texMappingGPU)
	{
		if(useNormalForMapping)
		{
			if(mappingKind == 0)
			{
				uv = CylindericalMapping(raw_normal);
			}
			else if(mappingKind == 1)
			{
				uv = SphericalMapping(raw_normal);
			}
			else if(mappingKind == 2)
			{
				uv = CubeMapping(raw_normal);
			}
		}
		else
		{
			if(mappingKind == 0)
			{
				uv = CylindericalMapping(raw_position);
			}
			else if(mappingKind == 1)
			{
				uv = SphericalMapping(raw_position);
			}
			else if(mappingKind == 2)
			{
				uv = CubeMapping(raw_position);
			}
		}
	}
	else
	{
		uv = vs_texcoord;
	}

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

	float S = (zFar - distance(vs_position, cameraPos))/(zFar - zNear); 
	fs_color = S*fs_color + vec4((1.f-S)*fogColor, 1.f);
}