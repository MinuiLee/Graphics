/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phong_shading_fragment.glsl
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

//////////////////////// Assignment 3

uniform sampler2D cubeTex[6];
uniform float K_t;
uniform bool shouldReflect;
uniform bool shouldRefract;
uniform bool shouldEnvPhong;
uniform bool activateRefractRGB;
uniform vec3 Eta;

float FresnelPower = 5.0f;

in vec3 RefractR;
in vec3 RefractG;
in vec3 RefractB;

// Ambient
vec3 calculateAmbient(Light light)
{
	return light.ambient;// * texture(material.diffuseTex, uv).rgb;
}

//Diffuse
vec3 calculateDiffuse(Light light, vec3 lightDir)
{
	float diffuse = max(dot(normalize(vs_normal), lightDir), 0.0);
	vec3 diffuseFinal = light.diffuse * diffuse;// * texture(material.diffuseTex, uv).rgb;

	return diffuseFinal;
}

// Speculate
vec3 calculateSpeculate(Light light, vec3 lightDir)
{
	vec3 nNormal = normalize(vs_normal);
	vec3 viewDir = normalize(cameraPos - vs_position);
	vec3 reflectDir = 2*nNormal*dot(nNormal,lightDir)-lightDir;
	float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);
	vec3 specular =  light.specular * spec;// * texture(material.specularTex, uv).rgb;

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

vec4 CalculateCubeMap(vec3 vector)
{
	vec3 abs = abs(vector);
	float max_val = max(max(abs.x, abs.y), abs.z);

	if(max_val == abs.x)
	{
		float neg = step(vector.x, 0.0);
		vec3 uv = vec3(vector.zy, abs.x) * vec3(mix(-1.0, 1.0, neg), -1, 1);
		
		return texture(cubeTex[int(neg)], vec2(uv.xy / uv.z + 1) * 0.5);
	}
	else if(max_val == abs.y)
	{
		float neg = step(vector.y, 0.0);
        vec3 uuv = vec3(vector.xz, abs.y) * vec3(1.0, mix(1.0, -1.0, neg), 1);
        
        vec2 uv = vec2(uuv.xy / uuv.z + 1) * 0.5;
        uv = uv.yx;
        
        if(vector.y < 0)
        {
            uv.x = 1 - uv.x;
            return texture(cubeTex[3-int(0)], uv);
        }
        else
        {
            uv.y = 1 - uv.y;
            return texture(cubeTex[3-int(1)], uv);
        }
	}
	else// if(max_val == abs.z)
	{
		float neg = step(vector.z, 0.0);
		vec3 uv = vec3(vector.xy, abs.z) * vec3(mix(1.0, -1.0, neg), -1, 1);
		
		return texture(cubeTex[4+int(neg)], vec2(uv.xy / uv.z + 1) * 0.5);
	}
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
			else
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
			else
			{
				uv = CubeMapping(raw_position);
			}
		}
	}
	else
	{
		uv = vs_texcoord;
	}

	vec3 phongColor = vec3(0.f);

	for(int i = 0; i < numLights; i++)
	{
		if(lights[i].type == 0) //directional
			phongColor += getDirectionalLight(lights[i]).xyz;
		else if(lights[i].type == 1) //point
			phongColor += getPointLight(lights[i]).xyz;
		else if(lights[i].type == 2) //spotlight
			phongColor += getSpotlight(lights[i]).xyz;
	}

	for(int i = 0; i < numLights; i++)
	{
		phongColor += globalAmbient * lights[i].ambient;
	}

	float S = (zFar - distance(vs_position, cameraPos))/(zFar - zNear); 
	phongColor = S * phongColor + (1.f - S) * fogColor;

	///////////////////////////////////////////

	vec3 I = normalize(vs_position - cameraPos);
	float N_dot_I = dot(vs_normal, I);
	float K = 1.f / K_t;
	
	vec3 Refract = vec3(0.f);
	vec3 refractColor;
	vec3 Reflect;

	float part = 1.0f - K * K * (1.0f - N_dot_I * N_dot_I);
	if(part >= 0)
			Refract =  (K * (N_dot_I) + sqrt(part)) * vs_normal - K * I;

	if(activateRefractRGB)
	{
		refractColor.r = CalculateCubeMap(RefractR).r;
		refractColor.g = CalculateCubeMap(RefractG).g;
		refractColor.b = CalculateCubeMap(RefractB).b;
	}
	else
	{
		refractColor = CalculateCubeMap(Refract).xyz;
	}
	
	Reflect = 2.0 * N_dot_I * vs_normal - I; 
	Reflect.x *= -1.f;
	vec3 reflectColor = CalculateCubeMap(Reflect).xyz;
	
	if(activateRefractRGB) K = 1.f/Eta[1];
	float F = ((1.0-K) * (1.0-K)) / ((1.0+K) * (1.0+K));

	float Ratio = F + (1.0 - F) * pow(1.0 - dot(normalize(cameraPos - vs_position), vs_normal), FresnelPower);
	vec3 mixCol = mix(refractColor, reflectColor, Ratio);
	
	phongColor = mix(refractColor, phongColor, 0.5);

	if(shouldReflect && shouldRefract)
		fs_color = vec4(mixCol, 1.f);
	else if(!shouldReflect && shouldRefract)
		fs_color = vec4(refractColor, 1.f);
	else if(shouldReflect && !shouldRefract)
		fs_color = vec4(reflectColor, 1.f);
	else if(shouldEnvPhong)
		fs_color = vec4(phongColor, 1.f);
}