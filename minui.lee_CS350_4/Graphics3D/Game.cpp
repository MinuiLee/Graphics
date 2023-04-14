/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Game.cpp
Purpose: Game class cpp file that handles all other classes to create a project.
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_2
Author: Minui Lee, minui.lee, 180002020
Creation date: 16 Sep 2020
End Header --------------------------------------------------------*/

#include "Game.h"
#include <filesystem>
#include <iostream>

Game::Game(const char* title) :
	window(nullptr), framebufferWidth(WINDOW_WIDTH), framebufferHeight(WINDOW_HEIGHT),
	dt(0.f), curTime(0.f), lastTime(0.f),
	camera(glm::vec3(0.f, 3.f, 10.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)),
	camPosition(glm::vec3(0.f, 3.f, 10.f)),
	worldUp(glm::vec3(0.f, 1.f, 0.1f)),
	camFront(glm::vec3(0.f, 0.f, -1.f)),
	fov(45.f), nearPlane(0.1f), farPlane(100.f),
	drawNormal(false), drawFaceNormal(false)
{
	mouseX = mouseY = 0.0;
	lastMouseX = lastMouseY = 0.0;
	mouseOffsetX = mouseOffsetY = 0.0;
	firstMouse = true;
	fogColor[0] = 0.f;
	fogColor[1] = 0.f;
	fogColor[2] = 0.f;

	srand(static_cast<unsigned int>(time(nullptr)));

	Initialize(title);
	InitImGUI();
	InitMatrices();
	InitShaders();
	InitTextures();
	InitMaterials();
	InitLights();
	InitModels();
	InitUniforms();

	camera.AddRotation(-13.f, 0.f, 0.f);

	////////////////////////////////////////////

	gBuffer = new GBuffer();
	gBuffer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
	shaders[SHADER_PHONG_SHADING]->Set1i(0, "gPosition");
	shaders[SHADER_PHONG_SHADING]->Set1i(1, "gNormal");
	shaders[SHADER_PHONG_SHADING]->Set1i(2, "gAlbedoSpec");

	octree = new Octree(meshes);
	//bsptree = new BSPTree(meshes, &planes);
}

Game::~Game()
{
	glfwDestroyWindow(window);
	glfwTerminate();

	for (size_t i = 0; i < shaders.size(); i++)
		delete shaders[i];

	for (size_t i = 0; i < textures.size(); i++)
		delete textures[i];

	for (size_t i = 0; i < meshes.size(); i++)
		delete meshes[i];

	for (size_t i = 0; i < materials.size(); i++)
		delete materials[i];

	for (size_t i = 0; i < lights.size(); i++)
		delete lights[i];

	ImGui_ImplOpenGL4_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

}

void Game::Update()
{
	UpdateDt();
	UpdateInput();

	for (Object* light : lightObjs)
	{
		if (light->GetCanRotate())
			light->MoveByAngle(0.01f, 2.5f);
	}

	if (bs)
	{
		if(!octree->IsIntersecting())
			bs->Update();
		octree->IntersectTest(bs);
	}
}

void Game::Render()
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	UpdateUniforms();

	///////////////////////////////////////////////////

	RenderDeferredObjects();
	quadObj->RenderObjectInNDC(shaders[SHADER_PHONG_SHADING], quadMesh, GL_TRIANGLE_STRIP);

	if (depth_copy)
		gBuffer->CopyDepthBuffer();

	RenderDebugObjects();

	///////////////////////////////////////////////////

	UpdateImGUI();

	glfwSwapBuffers(window);
	glFlush();

	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::UpdateDt()
{
	curTime = static_cast<float>(glfwGetTime());
	dt = curTime - lastTime;
	lastTime = curTime;
}

void Game::UpdateInput()
{
	glfwPollEvents();
	UpdateKeyboardInput();
	UpdateMouseInput();
	//this->camera.UpdateInput(dt, -1, this->mouseOffsetX, this->mouseOffsetY);
}

static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

void Game::UpdateImGUI()
{
	ImGui_ImplOpenGL4_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Editor");

	if (ImGui::Button("Recompile Shader"))
	{
		InitShaders();
	}

	if (ImGui::CollapsingHeader("Assignment 1"))
	{
		if (!meshes.empty())
		{
			if (ImGui::SliderInt("Meshes", &meshNum, 0, meshes.size()-1))
			{
				drawMeshes = false;
			}
		}
		ImGui::Checkbox("Draw Vertex Normals", &drawNormal);
		ImGui::Checkbox("Draw Face Normals", &drawFaceNormal);
		ImGui::Checkbox("Depth Copy", &depth_copy);
	}

	if (ImGui::CollapsingHeader("Assignment 2"))
	{
	}

	if (ImGui::CollapsingHeader("Assignment 3"))
	{
		ImGui::Checkbox("Draw Meshes", &drawMeshes);
		ImGui::Checkbox("Draw Octree AABB", &drawOctreeAABB);
		//ImGui::Checkbox("Draw Octree Colorful", &drawOctreeColorful);
		//ImGui::Checkbox("Draw BSP tree", &drawBSPTree);
	}

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL4_RenderDrawData(ImGui::GetDrawData());
}

void Game::UpdateMouseInput()
{
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (firstMouse)
	{
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		firstMouse = false;
	}

	mouseOffsetX = mouseX - lastMouseX;
	mouseOffsetY = mouseY - lastMouseY;

	lastMouseX = mouseX;
	lastMouseY = mouseY;
}

void Game::UpdateKeyboardInput()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	//Camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.Move(dt * 1.5f, FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.Move(dt * 1.5f, BACKWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.Move(dt * 1.5f, LEFT);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.Move(dt * 1.5f, RIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		camera.Move(dt * 1.5f, UP);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		camera.Move(dt * 1.5f, DOWN);
	}
	
	bool old = spaceKeyState;
	spaceKeyState = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if(spaceKeyState == true && old == false)
	{
		if (bs)
		{
			if (octree->IsIntersecting())
			{
				bs->Update();
			}
			else
			{
				delete bs;
				bs = nullptr;
			}
		}
		else
		{
			bs = new BoundingSphere();
			bs->Initialize(camera.GetPosition(), 0.1f);

			glm::mat4 temp = camera.GetViewMatrix();
			bs->dir = glm::normalize(glm::vec3(temp[2][0], temp[2][1], -temp[2][2])) * 0.1f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		camera.AddRotation(static_cast<float>(-mouseOffsetY / 2.f), static_cast<float>(mouseOffsetX / 2.f), 0.f);
	}
}

void Game::UpdateUniforms()
{
	ViewMatrix = camera.GetViewMatrix();

	//Update framebuffer size and projection matrix
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

	if (framebufferHeight)
	{
		ProjectionMatrix = glm::perspective(glm::radians(fov),
			static_cast<float>(framebufferWidth / framebufferHeight),
			nearPlane, farPlane);
	}

	shaders[SHADER_PHONG_SHADING]->Set1i(numLights, "numLights");
	shaders[SHADER_PHONG_SHADING]->SetVec3f(fogColor, "fogColor");
	shaders[SHADER_PHONG_SHADING]->SetVec3f(globalAmbient, "globalAmbient");
	shaders[SHADER_PHONG_SHADING]->SetVec3f(camera.GetPosition(), "cameraPos");
	shaders[SHADER_PHONG_SHADING]->Set1i(0, "gPosition");
	shaders[SHADER_PHONG_SHADING]->Set1i(1, "gNormal");
	shaders[SHADER_PHONG_SHADING]->Set1i(2, "gAlbedoSpec");

	shaders[SHADER_BASIC]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_BASIC]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");

	shaders[SHADER_GBUFFER]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_GBUFFER]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
}

void Game::Initialize(const char* title)
{
	//Init GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "ERROR::GLFW INIT FAILED \n";
		glfwTerminate();
	}

	//Init Window
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VERSION_MINOR);
	glfwWindowHint(GLFW_RESIZABLE, false); //resizable

	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //for MAX OS

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title, nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "ERROR::GLFW CREATE WINDOW FAILED\n";
		glfwTerminate();
	}

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, Game::Framebuffer_resize_callback);

	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::GLEW INIT FAILED \n";
		glfwTerminate();
	}

	//Input
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Game::InitImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL4_Init("#version 450");
	ImGui::StyleColorsDark();
}

void Game::InitMatrices()
{
	ViewMatrix = glm::mat4(1.f);
	ViewMatrix = glm::lookAt(camPosition, camPosition + camFront, worldUp);

	ProjectionMatrix = glm::mat4(1.f);
	ProjectionMatrix = glm::perspective(
		glm::radians(fov),
		static_cast<float>(framebufferWidth / framebufferHeight),
		nearPlane,
		farPlane
	);
}

void Game::InitShaders()
{
	for (size_t i = 0; i < shaders.size(); i++)
		delete shaders[i];
	shaders.clear();

	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"Shaders/basic_vertex.glsl", "Shaders/basic_fragment.glsl"));

	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"Shaders/phong_shading_vertex.glsl", "Shaders/phong_shading_fragment.glsl"));

	shaders.push_back(new Shader(VERSION_MAJOR, VERSION_MINOR,
		"Shaders/g_buffer_vertex.glsl", "Shaders/g_buffer_fragment.glsl"));
}

void Game::InitTextures()
{
	textures.push_back(new Texture("Images/metal_roof_diff_512x512.ppm", GL_TEXTURE_2D));
	textures.push_back(new Texture("Images/metal_roof_spec_512x512.ppm", GL_TEXTURE_2D));
}

void Game::InitMaterials()
{
	materials.push_back(new Material(32.f, 0, 1));
}

void Game::InitModels()
{
	std::string name;
	std::stringstream ss;
	for (const auto& entry : std::filesystem::directory_iterator("models/"))
	{
		std::ifstream in_file(entry.path().string());
		std::string line = "";

		while (std::getline(in_file, line))
		{
			std::ifstream in_file("models/" + line);
			if (in_file)
			{
				meshes.push_back(new Mesh("models/" + line));
				std::cout << "FINISHED LOADING  " << line << std::endl;
			}
		}
	}

	centerObject = new Object(glm::vec3(0.f), materials[0], std::vector<Light*>(), textures[TEX_DIFF], textures[TEX_SPEC]);

	lightMesh = new Mesh(new Sphere(36, 36, 0.1f));

	float step = 2 * PI / numLights;

	for (int i = 0; i < numLights; ++i)
	{
		lightObjs.push_back(new Object(glm::vec3(0.f), materials[0], std::vector<Light*>(), nullptr, nullptr));
		lightObjs.back()->SetColor(glm::vec3(255));
		lightObjs.back()->SetPosition(lights[i]->GetPosition());
		lightObjs.back()->SetAngle(step * i);
		lights[i]->GetDiffuse() = glm::vec3(255);
		lights[i]->UpdateDiffuseF();
	}

	quadMesh = new Mesh(new Quad());
	quadObj = new Object(glm::vec3(0.f), materials[0], std::vector<Light*>(), nullptr, nullptr);
}

void Game::InitLights()
{
	float dis = 2.5f;
	float step = 2 * PI / numLights;

	for (int i = 0; i < numLights; ++i)
	{
		lights.push_back(new Light(glm::vec3{ dis * cosf(i * step), 0.f, dis * sinf(i * step) }, glm::vec3(0.f),
			0.1f / numLights, 0.4f / numLights, 1.f / numLights,
			8.f, 8.f, LIGHTTYPE::POINT));
	}
}

void Game::InitUniforms()
{
	shaders[SHADER_PHONG_SHADING]->Set1i(numLights, "numLights");
	shaders[SHADER_PHONG_SHADING]->SetVec3f(fogColor, "fogColor");
	shaders[SHADER_PHONG_SHADING]->SetVec3f(globalAmbient, "globalAmbient");
	shaders[SHADER_PHONG_SHADING]->Set1i(0, "gPosition");
	shaders[SHADER_PHONG_SHADING]->Set1i(1, "gNormal");
	shaders[SHADER_PHONG_SHADING]->Set1i(2, "gAlbedoSpec");

	shaders[SHADER_BASIC]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_BASIC]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");

	shaders[SHADER_GBUFFER]->SetMat4fv(ViewMatrix, "ViewMatrix");
	shaders[SHADER_GBUFFER]->SetMat4fv(ProjectionMatrix, "ProjectionMatrix");
}

void Game::Framebuffer_resize_callback(GLFWwindow* /*window*/, int fbw, int fbh)
{
	glViewport(0, 0, fbw, fbh);
}

int Game::GetWindowShouldClose() const { return glfwWindowShouldClose(window); }
void Game::SetWindowShouldClose() const { glfwSetWindowShouldClose(window, GLFW_TRUE); }

//------------------------------------------ CS350 -------------------------------------//

void Game::RenderDeferredObjects()
{
	GeomatryPass();
	LightingPass();
}

void Game::RenderDebugObjects()
{
	if (drawNormal)
	{
		for (auto& m : meshes)
		{
			if (m)
				centerObject->RenderNormal(shaders[SHADER_BASIC], m);
		}
	}

	if (drawFaceNormal)
	{
		for (auto& m : meshes)
		{
			if (m)
				centerObject->RenderFaceNormal(shaders[SHADER_BASIC], m);
		}
	}

	/*if (drawBSPTree)
	{
		bsptree->Draw(shaders[SHADER_BASIC]);
	}*/

	if (drawOctreeAABB)
	{
		octree->DrawAABB(shaders[SHADER_BASIC]);
	}

	if (drawOctreeColorful)
	{
		octree->DrawTriangles(shaders[SHADER_BASIC]);
	}

	if(bs)
		bs->Draw(shaders[SHADER_BASIC], glm::vec3(0.f, 1.f, 1.f));
}

void Game::GeomatryPass()
{
	gBuffer->Bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UpdateUniforms();

	if (drawMeshes)
	{
		for (Mesh* m : meshes)
			centerObject->Render(shaders[SHADER_GBUFFER], m);
	}
	else
	{
		if(!meshes.empty())
			centerObject->Render(shaders[SHADER_GBUFFER], meshes[meshNum]);
	}
	gBuffer->UnBind();
}

void Game::LightingPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaders[SHADER_PHONG_SHADING]->Use();

	gBuffer->ActiveBindTexture();

	materials[0]->sendShiniesssToShader(*shaders[SHADER_PHONG_SHADING]);

	int size = static_cast<int>(lights.size());
	for (int i = 0; i < size; ++i)
	{
		lights[i]->sendToShader(*shaders[SHADER_PHONG_SHADING], i);
	}
}