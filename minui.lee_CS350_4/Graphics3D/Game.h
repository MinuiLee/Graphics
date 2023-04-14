/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Game.h
Purpose: Game class header that handles all other classes to create a project.
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#pragma once
#include <unordered_map>

#include "Light.h"

constexpr int VERSION_MAJOR = 4;
constexpr int VERSION_MINOR = 5;
constexpr int WINDOW_WIDTH = 1080;
constexpr int WINDOW_HEIGHT = 1080;

//include glew before glfw
#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <vec2.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <thread>

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Camera.h"
#include "Object.h"
#include "GBuffer.h"

// primitives
#include "AABB.h"
#include "BoundingSphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Ray.h"

#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl4.h"
#include "Point3D.h"

#include "Octree.h"
#include "BSPTree.h"

enum SHADER
{
	SHADER_BASIC = 0,
	SHADER_PHONG_SHADING,
	SHADER_GBUFFER
};

enum TEXTURE
{
	TEX_DIFF = 0,
	TEX_SPEC
};

enum MATERIAL
{
	MAT_1 = 0
};

enum MESH
{
	MESH_QUAD = 0
};

class Game
{
public:
	Game(const char* title);
	virtual ~Game();

	int GetWindowShouldClose() const;
	void SetWindowShouldClose() const;
	void Update();
	void Render();

	static void Framebuffer_resize_callback(GLFWwindow* window, int fbw, int fbh);
private:
	GLFWwindow* window;

	int framebufferWidth;
	int framebufferHeight;

	float dt;
	float curTime;
	float lastTime;

	double lastMouseX;
	double lastMouseY;
	double mouseX;
	double mouseY;
	double mouseOffsetX;
	double mouseOffsetY;
	bool firstMouse;

	Camera camera;

	glm::mat4 ViewMatrix;
	glm::vec3 camPosition;
	glm::vec3 worldUp;
	glm::vec3 camFront;

	glm::mat4 ProjectionMatrix;
	float fov;
	float nearPlane;
	float farPlane;

	std::vector<Shader*> shaders;
	std::vector<Texture*> textures;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	std::vector<Light*> lights;

	std::vector<Object*> lightObjs;
	Object* centerObject;
	Mesh* lightMesh;
	Object* quadObj;
	Mesh* quadMesh;

	bool drawNormal;
	bool drawFaceNormal;

	int numLights = 8;
	float fogColor[3];
	float globalAmbient[3];
	SHADER shaderType = SHADER::SHADER_PHONG_SHADING;

	void Initialize(const char* title);
	void InitImGUI();
	void InitMatrices();
	void InitShaders();
	void InitTextures();
	void InitMaterials();
	void InitModels();
	void InitLights();
	void InitUniforms();
	void UpdateUniforms();
	void UpdateKeyboardInput();
	void UpdateMouseInput();
	void UpdateImGUI();
	void UpdateDt();
	void UpdateInput();

private:
	//----------------- CS350 ----------------//

	void RenderDeferredObjects();
	void RenderDebugObjects();
	void GeomatryPass();
	void LightingPass();

	bool depth_copy = true;
	GBuffer* gBuffer;

	Octree* octree;
	BSPTree* bsptree;

	std::vector<Plane> planes;

	int meshNum = 0;

	bool drawMeshes = false;
	bool drawOctreeAABB = false;
	bool drawOctreeColorful = false;
	bool drawBSPTree = false;

	BoundingSphere * bs;
	bool spaceKeyState = false;
};

